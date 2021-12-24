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

/* RTOS Stuff */
QueueHandle_t xTagDataQueue;
SemaphoreHandle_t xTagDataQueueMutex;

/* Get Data from TPMS Tags and Send to SD Card task to write to SD Card */
void ble_client_task(void* pvParams)
{
	for(;;)
	{
		/* Routine Task after some time to Scan for TPMS Tags */

		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}
