/*
 * sdcard.c
 *
 *  Created on: 24-Dec-2021
 *      Author: Treel
 */


#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"
#include "sdcard.h"
#include "app_config.h"
#include "nmea_parser.h"
#include "gps_task.h"
#include "treel_tag.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


#define MOUNT_POINT "/sdcard"
static const char *SD_TAG = "SD CARD";
char sd_buffer[250];
// Pin mapping
#if CONFIG_IDF_TARGET_ESP32

#define CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED	ON

#define PIN_NUM_MISO 19//12//2//12
#define PIN_NUM_MOSI 23//13//15//13
#define PIN_NUM_CLK  18//14//14//14
#define PIN_NUM_CS   5//15//13//15
#endif

#define SPI_DMA_CHAN    1


//Global variables
sdcard_handle sd_handle;
sdmmc_card_t *card;
const char mount_point[] = MOUNT_POINT;
sdmmc_host_t host = SDSPI_HOST_APP();
file_handle gps_file_handle;


//externs
extern QueueHandle_t xGPSQueue;
extern m_gps_handle gps_handle;
void sdcard_Task(void* pvParams)
{
	gps_queue_msg gps;

	//gps = malloc(sizeof(struct gps_queue_msg *));
	for(;;)
	{
		if(sd_handle.mounted == true && gps_handle.init == true)
		{
			/* Get Data from GPS Queue */
			if(xQueueReceive(xGPSQueue, &gps, ( TickType_t ) 20 ) == pdPASS)
			{
				ESP_LOGI(SD_TAG,"\n%d/%d/%d %2d:%02d:%02d\tlatitude   = %.05f°N\tlongitude = %.05f°E\r\n",gps.date.day,gps.date.month, gps.date.year,gps.tim.hour, gps.tim.minute, gps.tim.second, gps.lat, gps.longi);
				//Write to SD Card
				if(sd_handle.mounted == true && sd_handle.busy == false)
				{
					sprintf(sd_buffer,"%02d/%02d/%d %02d:%02d:%02d\t%.05f\t%.05f\n",gps.date.day,gps.date.month, gps.date.year,gps.tim.hour, gps.tim.minute, gps.tim.second, gps.lat, gps.longi);
					write_data_to_file("GPS.txt",sd_buffer);
					gps_file_handle.if_exist = true;
					gps_file_handle.file_read = false;

				}
				else
				{
					//SD Card busy
					ESP_LOGE(SD_TAG,"Error SD Card not mounted or Busy");
				}
			}

		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}



app_ret sdcard_init(void)
{
	sd_handle_init();
    esp_err_t ret;

    //SD busy flag
    sd_handle.busy = true;
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ESP_LOGI(SD_TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(SD_TAG, "Using SPI peripheral");


    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(SD_TAG, "Failed to initialize bus.");
        return app_error;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_APP();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(SD_TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(SD_TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(SD_TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return app_error;
    }
    ESP_LOGI(SD_TAG, "Filesystem mounted");
    sd_handle.mounted = true;
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    //SD busy flag
    sd_handle.busy = false;

    return app_success;
}

void sd_handle_init(void)
{
	sd_handle.mounted = false;
	sd_handle.busy = false;
}
app_ret create_file(char* filename)
{
#if 1
	struct stat st;
	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);

	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		if (stat(file, &st) == 0) {
			// Delete it if it exists
			unlink(file);
		}
		ESP_LOGI(SD_TAG, "Opening file %s", file);
		FILE *f = fopen(file, "w");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for writing");
			sd_handle.busy = false;
			return app_error;
		}
		fclose(f);
		//SD busy flag
		sd_handle.busy = false;
	}
	else
	{
		return app_error;

	}
	return app_success;
#endif
#if 0
	if(sd_handle.mounted == true && sd_handle.busy == false){
	FILE* f = fopen(MOUNT_POINT"/hello.txt", "w");
	if (f == NULL) {
		ESP_LOGE(TAG, "Failed to open file for writing");
		return app_error;
	}
	fprintf(f, "Hello %s!\n", card->cid.name);
	fclose(f);
	ESP_LOGI(TAG, "File written");
	}
	else
	{
		return app_error;
	}

	return app_success;
#endif
}


app_ret delete_file(char* filename)
{
	struct stat st;
	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);

	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Deleting file %s", file);
		if (stat(file, &st) == 0) {
			// Delete it if it exists
			unlink(file);
		}
		//SD busy flag
		sd_handle.busy = false;
	}
	else
	{
		return app_error;
	}

	return app_success;
}

/* Writes Data in an appendig Mode keep that in mind*/
app_ret write_data_to_file(char* filename, char* data)
{
	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);

	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Writing to file %s", filename);
		FILE *f = fopen(file, "a");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for writing");
			sd_handle.busy = false;
			return app_error;
		}

		//Write Data
		fprintf(f, data);
		fclose(f);
		ESP_LOGI(SD_TAG, "File written");
		//SD busy flag
		sd_handle.busy = false;

	}
	else
	{
		return app_error;
	}

	return app_success;

}

app_ret read_data_from_file(char* filename, char* buffer, size_t data_size)
{
	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);

	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Reading file %s", file);
		FILE *f = fopen(file, "r");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for reading");
			sd_handle.busy = false;
			return app_error;
		}

		fgets(buffer, data_size, f);
		fclose(f);
		//SD busy flag
		sd_handle.busy = false;
	}
	else
	{
		return app_error;
	}

	return app_success;

}


app_ret get_no_lines_to_tx(char* filename, file_handle* file_to_handle)
{
	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);
	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Reading no of lines to tx from File %s", file);
		FILE *f = fopen(file, "r");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for reading");
			sd_handle.busy = false;
			return app_error;
		}
		do
		{
			if(fgetc(f) == '\n')
			{
				//New line
				file_to_handle->total_lines++;

			}
			if(feof(f))
			{
				break;
			}
		}while(1);
	}
	sd_handle.busy = false;
	return app_success;
}

app_ret read_line(char* filename,file_handle* file_to_handle, char* buffer, size_t data_size)
{
	char* file ;
	char temp;
	int i;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);

	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Reading file %s", file);
		FILE *f = fopen(file, "r");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for reading");
			sd_handle.busy = false;
			return app_error;
		}
		if(file_to_handle->file_type == GPS_FILE_TYPE)
		{
			if(file_to_handle->current_line != 0)
			{
				//Set the position
				 fseek( f, (file_to_handle->current_line*GPS_BUFFER_LEN),SEEK_SET);
			}
			//GPS File handling
			for (i = 0; i < GPS_BUFFER_LEN; i++)
			{
				temp = fgetc(f);
				*buffer++ = temp;
			}
			if(temp == '\n')
			{
				//Last byte of the buffer must be new line then valid buffer
				file_to_handle->current_line++;
				*buffer = '\0';
				ESP_LOGI(SD_TAG, "Successfully copied the Buffer");
				file_to_handle->valid_data = true;
				fclose(f);
				sd_handle.busy = false;
				return app_success;
			}
			else if(feof(f))
			{
				fclose(f);
				//Delete the File
				//delete_file(filename);
				ESP_LOGI(SD_TAG, "Successfully copied the Buffer");
				file_to_handle->file_read = true;
				sd_handle.busy = false;
				return app_success;

			}
			else
			{
				ESP_LOGE(SD_TAG, "Corrupt GPS File!");
				fclose(f);
				sd_handle.busy = false;
				return app_error;
			}

		}
		else if(file_to_handle->file_type == TAG_FILE_TYPE)
		{
			//Tag file handling
		}
		sd_handle.busy = false;
	}

	return app_error;
}

app_ret rename_file(char* file_to_rename, char* rename_file)
{

	char* file_s ;
	file_s = malloc(strlen(MOUNT_POINT)+strlen(file_to_rename) +3);
	sprintf(file_s,"%s/%s",MOUNT_POINT,file_to_rename);

	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(rename_file) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,rename_file);

	// Check if destination file exists before renaming
	struct stat st;
	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		if (stat(file, &st) == 0) {
			// Delete it if it exists
			unlink(file);
		}

		// Rename original file
		ESP_LOGI(SD_TAG, "Renaming file %s to %s", file_to_rename, rename_file);
		if (rename(file_s, file) != 0) {
			ESP_LOGE(SD_TAG, "Rename failed");
			sd_handle.busy = false;
			return app_error;
		}
		//SD busy flag
		sd_handle.busy = false;
	}
	else
	{
		return app_error;
	}

	return app_success;

}

app_ret if_exists(char* filename)
{
	char* file ;
	file = malloc(strlen(MOUNT_POINT)+strlen(filename) +3);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);

	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		sd_handle.busy = true;
		FILE *f;
		if((f = fopen(file,"r")))
		{
			fclose(f);
			sd_handle.busy = false;
			return app_success;
		}
		sd_handle.busy = false;
	}
	return app_error;
}
app_ret sdcard_deinit()
{
	//SD busy flag
	sd_handle.busy = true;
	// All done, unmount partition and disable SPI peripheral
	esp_vfs_fat_sdcard_unmount(mount_point, card);
	sd_handle.mounted = false;
	ESP_LOGI(SD_TAG, "Card unmounted");

	//deinitialize the bus after all devices are removed
	spi_bus_free(host.slot);
	//SD busy flag
	sd_handle.busy = false;
	return app_success;
}




