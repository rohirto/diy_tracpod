/*
 * ble_client.c
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
#include "app_config.h"
#include "treel_tag.h"
#include "esp_gap_ble_api.h"
#include "ble_tasks.h"

/* RTOS Stuff */
QueueHandle_t xTagDataQueue;
SemaphoreHandle_t xTagDataQueueMutex;

/* Global Variables */
ble_client_handle client_handle;
static esp_ble_scan_params_t ble_scan_params_1 = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};
/* Externs */
extern treel_tag_data app_tag_r, app_tag_f;
/* Get Data from TPMS Tags and Send to SD Card task to write to SD Card */
void ble_client_task(void* pvParams)
{
	tag_queue_msg msg_1;
	for(;;)
	{
		/* Routine Task after some time to Scan for TPMS Tags */
		/* Start BLE Scan */
		/* Get Notification from BLE Handler */
		if(app_tag_r.tag_detected == true)
		{
			if(filter_data(&app_tag_r) == app_success)
			{
				if(process_data(&app_tag_r) == app_success)
				{
					//Print Data
					//print_tag_data(&app_tag_r);
					msg_1.tag_type = REAR_TAG_TYPE;
					msg_1.tag_temperature = app_tag_r.tag_temperature;
					msg_1.tag_pressure = app_tag_r.tag_pressure;
					msg_1.tag_battery = app_tag_r.tag_battery;
					//Send to Queue
					xQueueSendToBack(xTagDataQueue, &msg_1, ( TickType_t ) 20 );
				}
			}
			app_tag_r.tag_detected = false;
		}
		if(app_tag_f.tag_detected == true)
		{
			if(filter_data(&app_tag_f) == app_success)
			{
				if(process_data(&app_tag_f) == app_success)
				{
					//Print
					//print_tag_data(&app_tag_f);
					msg_1.tag_type = FRONT_TAG_TYPE;
					msg_1.tag_temperature = app_tag_f.tag_temperature;
					msg_1.tag_pressure = app_tag_f.tag_pressure;
					msg_1.tag_battery = app_tag_f.tag_battery;
					//Send to Queue
					xQueueSendToBack(xTagDataQueue, &msg_1, ( TickType_t ) 20 );
				}
			}
			app_tag_f.tag_detected = false;
		}
		/* Process Buffer */
		/* Send to Tag Data Queue */
		/* Call back scan again */
		if(client_handle.scan_complete == true)
		{
			client_handle.scan_complete = false;
			esp_ble_gap_set_scan_params(&ble_scan_params_1);
		}
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}
