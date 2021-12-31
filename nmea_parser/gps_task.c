/*
 * gps_task.c
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
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "gps_task.h"
#include "nmea_parser.h"



//Extern
extern gps_queue_msg msg;
extern SemaphoreHandle_t xGPSQueueMutex;
extern QueueHandle_t xGPSQueue;

/* Get valid and latest GPS Data and send it to SD Card */
void gps_task(void* pvParams)
{
	for(;;)
	{
		gps_init();

		//if(msg.valid == true)
		{
			//if(msg.lat != 0 || msg.longi != 0)
			{
				/* Take the Mutex */
				xSemaphoreTake(xGPSQueueMutex, portMAX_DELAY);
				xQueueOverwrite(xGPSQueue, &msg);
				xSemaphoreGive( xGPSQueueMutex );

			}
//			//Valid GPS Data, Send to Queue
//			/* Take the Mutex */
//			xSemaphoreTake(xGPSQueueMutex, portMAX_DELAY);
//			{
//				/* Send data to mailbox */
//				xQueueOverwrite(xGPSQueue, &msg);
//			}
//			/* Give back the Mutex */
//			xSemaphoreGive( xGPSQueueMutex );
		}

		/* This should after a timer read the GPS Data and store it in SD */
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

void prvTimerCallback( TimerHandle_t xTimer )
{


}
