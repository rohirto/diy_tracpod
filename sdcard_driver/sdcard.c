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
#include "config.h"
#include "nmea_parser.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"


#define MOUNT_POINT "/sdcard"
static const char *SD_TAG = "SD CARD";

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
sdmmc_host_t host = SDSPI_HOST_DEFAULT();

//externs
extern QueueHandle_t xGPSQueue;

void sdcard_Task(void* pvParams)
{
	gps_queue_msg *gps;
	for(;;)
	{
		/* Get Data from GPS Queue */
		if(xQueueReceive(xGPSQueue, &gps, pdMS_TO_TICKS(500)) == pdPASS)
		{
			ESP_LOGI(SD_TAG,    "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
					"\t\t\t\t\t\tlongitude = %.05f°E\r\n",

					gps->lat, gps->longi);
			//Write to SD Card
		}

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
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;
    gpio_set_pull_mode(5, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes

    gpio_set_pull_mode(18, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(23, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(19, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

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
	//file = strcat((char*)MOUNT_POINT,(char*)filename);
	sprintf(file,"%s/%s",MOUNT_POINT,filename);
	ESP_LOGI(SD_TAG,"%s",file);
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
	const char* file =  strcat((char*)MOUNT_POINT,(char*)filename);
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


app_ret write_data_to_file(char* filename, char* data)
{
	const char* file =  strcat((char*)MOUNT_POINT,(char*)filename);
	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Opening file %s", file);
		FILE *f = fopen(file, "w");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for writing");
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
	const char* file =  strcat((char*)MOUNT_POINT,(char*)filename);
	if(sd_handle.mounted == true && sd_handle.busy == false)
	{
		//SD busy flag
		sd_handle.busy = true;
		ESP_LOGI(SD_TAG, "Reading file %s", file);
		FILE *f = fopen(file, "r");
		if (f == NULL) {
			ESP_LOGE(SD_TAG, "Failed to open file for reading");
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


app_ret rename_file(char* file_to_rename, char* rename_file)
{
	const char* file_s =  strcat((char*)MOUNT_POINT,(char*)file_to_rename);
	const char* file =  strcat((char*)MOUNT_POINT,(char*)rename_file);
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




