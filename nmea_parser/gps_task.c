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
#include "esp_log.h"
#include "gps_task.h"


void gps_task(void* pvParams)
{
	for(;;)
	{
		/* This should after a timer read the GPS Data and store it in flash */
	}
}

void prvTimerCallback( TimerHandle_t xTimer )
{

	/*Start ADC conversion and store the value */
	//ADC1_Start(&bslADC);
}
