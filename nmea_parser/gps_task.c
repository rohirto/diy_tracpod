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
#include "app_config.h"



//Extern
//extern gps_queue_msg msg;
//extern SemaphoreHandle_t xGPSQueueMutex;
//extern QueueHandle_t xGPSQueue;

m_gps_handle gps_handle;
const char* GPS_LOG	="GPS.txt";

/* Get valid and latest GPS Data and send it to SD Card */
void gps_task(void* pvParams)
{
	//gps_queue_msg *msg_1 = NULL;
	for(;;)
	{
		/* Calls GPS module every 10 secs then sleeps*/
		gps_init();

		/* This should after a timer read the GPS Data and store it in SD */
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

void prvTimerCallback( TimerHandle_t xTimer )
{


}
