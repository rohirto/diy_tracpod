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
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "gps_task.h"
#include "ble_tasks.h"
#include "sdcard.h"
#include "app_config.h"
#include "esp_gatts_api.h"

ble_server_handle server_handle_gps;
ble_server_handle server_handle_tag;
char gps_line[GPS_BUFFER_LEN + 1];
char tag_line[TAG_BUFFER_LEN + 1];
static const char *SERVER_TAG = "BLE_SERVER";
uint8_t curr_file = NO_CURR_FILE;
//Externs
extern file_handle gps_file_handle;
extern file_handle f_tag_file_handle;
extern file_handle r_tag_file_handle;
extern sdcard_handle sd_handle;
extern SemaphoreHandle_t xSDMutex;
extern SemaphoreHandle_t xFileMutex;


void ble_server_gpstask(void *pvParams)
{
	for(;;)
	{
		if(server_handle_gps.connected == true)
		{
			if(server_handle_gps.notify_enabled == true )
			{
				if(if_exists("GPS.txt") == app_success)
				{
					if(gps_file_handling() != app_success)
					{
						//Error
						delete_file("GPS.txt");
						gps_file_handle.if_exist = false;
						curr_file = NO_CURR_FILE;

					}
					else
					{
						curr_file = NO_CURR_FILE;
					}
				}
				else
				{
					ESP_LOGI(SERVER_TAG,"GPS File doesnot exists");
				}
			}
			else
			{
				ESP_LOGI(SERVER_TAG,"GPS Notify Disabled");
			}
		}
		else
		{
			ESP_LOGI(SERVER_TAG,"GPS No Device Connected");
		}
//			/* BLE Server Task -> After some interval it should connect with BLE Client (Phone) and synchronize the Data */
//			/* Each 10 mins should sync data with Mobile App */
//			switch(curr_file)
//			{
//			case NO_CURR_FILE:
//				ESP_LOGI(SERVER_TAG,"No file to be handled rigt now");
//				if(f_tag_file_handle.file_read != true && f_tag_file_handle.if_exist == true)
//				{
//					curr_file =  FRONT_CURR_FILE;
//				}
//				if (gps_file_handle.file_read != true && gps_file_handle.if_exist == true)
//				{
//					curr_file = GPS_CURR_FILE;
//				}
//
//				break;
//			case GPS_CURR_FILE:
//
//				//GPS Handling
//				if(server_handle_gps.notify_enabled == true )
//				{
//					if(1)
//					{
//						if(gps_file_handling() != app_success)
//						{
//							//Error
//							delete_file("GPS.txt");
//							gps_file_handle.if_exist = false;
//							curr_file = NO_CURR_FILE;
//
//						}
//						else
//						{
//							curr_file = NO_CURR_FILE;
//						}
//					}
//				}
//				else
//				{
//					ESP_LOGI(SERVER_TAG,"Notify Disabled");
//				}
//
//				break;
//			case FRONT_CURR_FILE:
//
//				if(server_handle_tag.notify_enabled == true )
//				{
//					if(1 )
//					{
//						if(f_tag_file_handling() != app_success)
//						{
//							//Error
//							delete_file("tag.txt");
//							f_tag_file_handle.if_exist = false;
//							curr_file = NO_CURR_FILE;
//
//						}
//						else
//						{
//							curr_file = NO_CURR_FILE;
//						}
//					}
//				}
//				else
//				{
//					ESP_LOGI(SERVER_TAG,"Notify Disabled");
//				}
//
//				break;
//			}
//		}
//		else
//		{
//			//No device is connected
//			ESP_LOGI(SERVER_TAG,"No Device Connected");
//
//		}


		//			//A device is connected
		//			if(server_handle.notify_enabled == true )
		//			{
		//				if(sd_handle.mounted == true && sd_handle.busy == false)
		//				{
		//					switch(server_handle.current_file)
		//					{
		//					case GPS_CURR_FILE:
		//						if(gps_file_handling() != app_success)
		//						{
		//							//Error
		//							delete_file("GPS.txt");
		//							gps_file_handle.if_exist = false;
		//
		//						}
		//						break;
		//					case FRONT_CURR_FILE:
		//						if(f_tag_file_handling() != app_success)
		//						{
		//							//Error
		//							delete_file("front.txt");
		//							f_tag_file_handle.if_exist = false;
		//						}
		//						break;
		//					case REAR_CURR_FILE:
		//						if(r_tag_file_handling() != app_success)
		//						{
		//							//Error
		//							delete_file("rear.txt");
		//							r_tag_file_handle.if_exist = false;
		//						}
		//						break;
		//					case NO_CURR_FILE:
		//						ESP_LOGI(SERVER_TAG,"No file to be handled rigt now");
		//						if(gps_file_handle.file_read != true && gps_file_handle.if_exist == true)
		//						{
		//							server_handle.current_file = GPS_CURR_FILE;
		//						}
		//						else if (f_tag_file_handle.file_read != true && f_tag_file_handle.if_exist == true)
		//						{
		//							server_handle.current_file = FRONT_CURR_FILE;
		//						}
		//						else if (r_tag_file_handle.file_read != true && r_tag_file_handle.if_exist == true)
		//						{
		//							server_handle.current_file = REAR_CURR_FILE;
		//						}
		//						break;
		//					default:
		//						break;
		//
		//					}
		//					//Switch case for File Handling , Need to sequentially handle this stuff
		//
		//					//f_tag_file_handling()
		//					//r_tag_file_handling()
		//
		//				}
		//				else
		//				{
		//					ESP_LOGI(SERVER_TAG,"SD CARD not mounted or Busy");
		//				}
		//
		//
		//			}
		//			else
		//			{
		//				ESP_LOGI(SERVER_TAG,"Notify Disabled");
		//			}
		//		}
		//		else
		//		{
		//			//No device is connected
		//			ESP_LOGI(SERVER_TAG,"No Device Connected");
		//
		//		}

		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

void ble_server_tagtask(void* pvParams)
{
	for(;;)
	{
		if(server_handle_tag.connected == true)
		{
			if(server_handle_tag.notify_enabled == true )
			{
				if(if_exists("tag.txt") == app_success)
				{
					if(f_tag_file_handling() != app_success)
					{
						//Error
						delete_file("tag.txt");
						f_tag_file_handle.if_exist = false;
						//curr_file = NO_CURR_FILE;

					}
					else
					{
						//curr_file = NO_CURR_FILE;
						vTaskDelay(pdMS_TO_TICKS(2000));
					}
				}
				else
				{
					ESP_LOGI(SERVER_TAG,"Tag File doesnot exists");
				}

			}
			else
			{
				ESP_LOGI(SERVER_TAG,"Tag Notify Disabled");
			}

		}
		else
		{
			ESP_LOGI(SERVER_TAG,"Tag No Device Connected");
		}
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

app_ret gps_file_handling()
{
	xSemaphoreTake(xFileMutex, portMAX_DELAY);
	xSemaphoreTake(xSDMutex, portMAX_DELAY);
//	if(gps_file_handle.if_exist == true && gps_file_handle.busy != true)
	if(gps_file_handle.if_exist == true)
	{
//		gps_file_handle.busy = true;
//		//Read the total line from SD Card
		gps_file_handle.total_lines = 0;
		get_no_lines_to_tx("GPS.txt", &gps_file_handle);
		ESP_LOGI(SERVER_TAG, "Total lines: %d", gps_file_handle.total_lines);
		while(gps_file_handle.file_read != true)
		{
			if(read_line("GPS.txt",&gps_file_handle,gps_line,sizeof(gps_line)) == app_success)
			{
				if(gps_file_handle.valid_data == true)
				{
					ESP_LOGI(SERVER_TAG,"GPS Data To be uploaded: %s",gps_line);
					memcpy(server_handle_gps.notify_data, gps_line, 38);
					//Write to Client
					//esp_ble_gatts_send_indicate(server_handle_gps.app_gatt_if, server_handle_gps.conn_id, server_handle_gps.char_handle,
					//		sizeof(server_handle_gps.notify_data), server_handle_gps.notify_data, false);
					//TODO
					//Implement just write to characterstics
					esp_ble_gatts_set_attr_value(server_handle_gps.char_handle,sizeof(server_handle_gps.notify_data),server_handle_gps.notify_data);
					gps_file_handle.valid_data = false;
				}
			}
			else
			{
				ESP_LOGI(SERVER_TAG,"GPS Data Read Error");
				server_handle_gps.current_file = NO_CURR_FILE;
//				gps_file_handle.busy = false;
				xSemaphoreGive( xSDMutex );
				xSemaphoreGive(xFileMutex);
				return app_error;
			}

		}
		if(gps_file_handle.file_read == true)
		{
//			//Delete the File
			delete_file("GPS.txt");
			gps_file_handle.if_exist = false;
			server_handle_gps.current_file = NO_CURR_FILE;
//			//gps_file_handle.file_read = false;
		}
//
	}
	else
	{
		ESP_LOGI(SERVER_TAG,"GPS Log File doesnot exists or File in Use");
		server_handle_gps.current_file = NO_CURR_FILE;		//Then handle next File
//		gps_file_handle.busy = false;
		xSemaphoreGive( xSDMutex );
		xSemaphoreGive(xFileMutex);
		return app_error;
	}
//	gps_file_handle.busy = false;
	xSemaphoreGive( xSDMutex );
	xSemaphoreGive(xFileMutex);
	return app_success;
}

app_ret f_tag_file_handling()
{
	xSemaphoreTake(xFileMutex, portMAX_DELAY);
	xSemaphoreTake(xSDMutex, portMAX_DELAY);
	if(f_tag_file_handle.if_exist == true)
	{
		//f_tag_file_handle.busy = true;
		//Read the total line from SD Card
		f_tag_file_handle.total_lines = 0;
		//get_no_lines_to_tx("front.txt", &f_tag_file_handle);
		get_no_lines_to_tx("tag.txt", &f_tag_file_handle);
		ESP_LOGI(SERVER_TAG, "Total lines: %d", f_tag_file_handle.total_lines);
		while(f_tag_file_handle.file_read != true)
		{
			if(read_line("tag.txt",&f_tag_file_handle,tag_line,sizeof(tag_line)) == app_success)
			{
				if(f_tag_file_handle.valid_data == true)
				{
					ESP_LOGI(SERVER_TAG,"Tag Data To be uploaded: %s",tag_line);
					memcpy(server_handle_tag.notify_data, tag_line, TAG_BUFFER_LEN);
					//Write to Client
					//esp_ble_gatts_send_indicate(server_handle_tag.app_gatt_if, server_handle_tag.conn_id, server_handle_tag.char_handle,
					//		sizeof(server_handle_tag.notify_data), server_handle_tag.notify_data, false);
					//TODO
					//Implement just write to characterstics
					esp_ble_gatts_set_attr_value(server_handle_tag.char_handle,sizeof(server_handle_tag.notify_data),server_handle_tag.notify_data);

					f_tag_file_handle.valid_data = false;
				}
			}
			else
			{
				ESP_LOGI(SERVER_TAG,"Tag Data Read Error");
				server_handle_tag.current_file = NO_CURR_FILE;
				//f_tag_file_handle.busy = false;
				xSemaphoreGive( xSDMutex );
				xSemaphoreGive( xFileMutex );
				return app_error;
			}
		}
		if(f_tag_file_handle.file_read == true)
		{
			//Delete the File
			delete_file("tag.txt");
			f_tag_file_handle.if_exist = false;
			server_handle_tag.current_file = NO_CURR_FILE;
			//gps_file_handle.file_read = false;
		}

	}
	else
	{
		ESP_LOGI(SERVER_TAG,"Tag Log File doesnot exists");
		server_handle_tag.current_file = NO_CURR_FILE;		//Then handle next File
		//f_tag_file_handle.busy = false;
		xSemaphoreGive( xSDMutex );
		xSemaphoreGive( xFileMutex );
		return app_error;
	}
	//f_tag_file_handle.busy = false;
	xSemaphoreGive( xSDMutex );
	xSemaphoreGive( xFileMutex );
	return app_success;

}
app_ret r_tag_file_handling()
{
//	xSemaphoreTake(xSDMutex, portMAX_DELAY);
//	if(r_tag_file_handle.if_exist == true && r_tag_file_handle.busy != true)
//	{
//		r_tag_file_handle.busy = true;
//		//Read the total line from SD Card
//		r_tag_file_handle.total_lines = 0;
//		get_no_lines_to_tx("rear.txt", &r_tag_file_handle);
//		ESP_LOGI(SERVER_TAG, "Total lines: %d", r_tag_file_handle.total_lines);
//		while(r_tag_file_handle.file_read != true)
//		{
//			if(read_line("rear.txt",&r_tag_file_handle,tag_line,sizeof(tag_line)) == app_success)
//			{
//				if(r_tag_file_handle.valid_data == true)
//				{
//					ESP_LOGI(SERVER_TAG,"Rear Data To be uploaded: %s",tag_line);
//					memcpy(server_handle.notify_data, tag_line, 15);
//					//Write to Client
//					esp_ble_gatts_send_indicate(server_handle.app_gatt_if, server_handle.conn_id, server_handle.char_handle,
//							sizeof(server_handle.notify_data), server_handle.notify_data, false);
//					r_tag_file_handle.valid_data = false;
//				}
//			}
//			else
//			{
//				ESP_LOGI(SERVER_TAG,"Rear Data Read Error");
//				server_handle.current_file = NO_CURR_FILE;
//				r_tag_file_handle.busy = false;
//				xSemaphoreGive( xSDMutex );
//				return app_error;
//			}
//		}
//		if(r_tag_file_handle.file_read == true)
//		{
//			//Delete the File
//			delete_file("rear.txt");
//			r_tag_file_handle.if_exist = false;
//			server_handle.current_file = NO_CURR_FILE;
//			//gps_file_handle.file_read = false;
//		}
//
//	}
//	else
//	{
//		ESP_LOGI(SERVER_TAG,"Rear Log File doesnot exists");
//		server_handle.current_file = NO_CURR_FILE;
//		r_tag_file_handle.busy = false;
//		xSemaphoreGive( xSDMutex );
//		return app_error;
//	}
//	r_tag_file_handle.busy = false;
//	xSemaphoreGive( xSDMutex );
	return app_success;

}
