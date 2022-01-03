/*
 * ble_server.c
 *
 *  Created on: 09-Dec-2021
 *      Author: Treel
 */


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "gps_task.h"
#include "ble_tasks.h"
#include "sdcard.h"
#include "app_config.h"
#include "esp_gatts_api.h"

ble_server_handle server_handle;
char gps_line[GPS_BUFFER_LEN + 1];
char tag_line[TAG_BUFFER_LEN + 1];
static const char *SERVER_TAG = "BLE_SERVER";

//Externs
extern file_handle gps_file_handle;
extern file_handle f_tag_file_handle;
extern file_handle r_tag_file_handle;
extern sdcard_handle sd_handle;

void ble_server_task(void *pvParams)
{
	for(;;)
	{
		/* BLE Server Task -> After some interval it should connect with BLE Client (Phone) and synchronize the Data */
		/* Each 10 mins should sync data with Mobile App */
		if(server_handle.connected == true)
		{
			//A device is connected
			if(server_handle.notify_enabled == true )
			{
				if(sd_handle.mounted == true && sd_handle.busy == false)
				{
					//Switch case for File Handling , Need to sequentially handle this stuff
					gps_file_handling();
					//f_tag_file_handling()
					//r_tag_file_handling()

				}
				else
				{
					ESP_LOGI(SERVER_TAG,"SD CARD not mounted or Busy");
				}


			}
			else
			{
				ESP_LOGI(SERVER_TAG,"Notify Disabled");
			}
		}
		else
		{
			//No device is connected
			ESP_LOGI(SERVER_TAG,"No Device Connected");

		}

		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

app_ret gps_file_handling()
{
	if(gps_file_handle.if_exist == true)
	{
		//Read the total line from SD Card
		gps_file_handle.total_lines = 0;
		get_no_lines_to_tx("GPS.txt", &gps_file_handle);
		ESP_LOGI(SERVER_TAG, "Total lines: %d", gps_file_handle.total_lines);
		while(gps_file_handle.file_read != true)
		{
			read_line("GPS.txt",&gps_file_handle,gps_line,sizeof(gps_line));
			if(gps_file_handle.valid_data == true)
			{
				ESP_LOGI(SERVER_TAG,"Data To be uploaded: %s",gps_line);
				memcpy(server_handle.notify_data, gps_line, 38);
				//Write to Client
				esp_ble_gatts_send_indicate(server_handle.app_gatt_if, server_handle.conn_id, server_handle.char_handle,
						sizeof(server_handle.notify_data), server_handle.notify_data, false);
				gps_file_handle.valid_data = false;
			}
		}
		if(gps_file_handle.file_read == true)
		{
			//Delete the File
			delete_file("GPS.txt");
			gps_file_handle.if_exist = false;
			//gps_file_handle.file_read = false;
		}

	}
	else
	{
		ESP_LOGI(SERVER_TAG,"GPS Log File doesnot exists");
		return app_error;
	}

	return app_success;
}

app_ret f_tag_file_handling()
{
	if(f_tag_file_handle.if_exist == true)
	{
		//Read the total line from SD Card
		f_tag_file_handle.total_lines = 0;
		get_no_lines_to_tx("GPS.txt", &f_tag_file_handle);
		ESP_LOGI(SERVER_TAG, "Total lines: %d", f_tag_file_handle.total_lines);
		while(f_tag_file_handle.file_read != true)
		{
			read_line("front.txt",&f_tag_file_handle,tag_line,sizeof(tag_line));
			if(f_tag_file_handle.valid_data == true)
			{
				ESP_LOGI(SERVER_TAG,"Data To be uploaded: %s",f_tag_file_handle);
				memcpy(server_handle.notify_data, tag_line, 15);
				//Write to Client
				esp_ble_gatts_send_indicate(server_handle.app_gatt_if, server_handle.conn_id, server_handle.char_handle,
						sizeof(server_handle.notify_data), server_handle.notify_data, false);
				f_tag_file_handle.valid_data = false;
			}
		}
		if(f_tag_file_handle.file_read == true)
		{
			//Delete the File
			delete_file("front.txt");
			f_tag_file_handle.if_exist = false;
			//gps_file_handle.file_read = false;
		}

	}
	else
	{
		ESP_LOGI(SERVER_TAG,"Front Log File doesnot exists");
		return app_error;
	}

	return app_success;

}
app_ret r_tag_file_handling()
{
	if(f_tag_file_handle.if_exist == true)
	{
		//Read the total line from SD Card
		r_tag_file_handle.total_lines = 0;
		get_no_lines_to_tx("GPS.txt", &r_tag_file_handle);
		ESP_LOGI(SERVER_TAG, "Total lines: %d", r_tag_file_handle.total_lines);
		while(r_tag_file_handle.file_read != true)
		{
			read_line("front.txt",&r_tag_file_handle,tag_line,sizeof(tag_line));
			if(r_tag_file_handle.valid_data == true)
			{
				ESP_LOGI(SERVER_TAG,"Data To be uploaded: %s",r_tag_file_handle);
				memcpy(server_handle.notify_data, tag_line, 15);
				//Write to Client
				esp_ble_gatts_send_indicate(server_handle.app_gatt_if, server_handle.conn_id, server_handle.char_handle,
						sizeof(server_handle.notify_data), server_handle.notify_data, false);
				r_tag_file_handle.valid_data = false;
			}
		}
		if(r_tag_file_handle.file_read == true)
		{
			//Delete the File
			delete_file("rear.txt");
			r_tag_file_handle.if_exist = false;
			//gps_file_handle.file_read = false;
		}

	}
	else
	{
		ESP_LOGI(SERVER_TAG,"Rear Log File doesnot exists");
		return app_error;
	}

	return app_success;

}
