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


void ble_server_task(void *pvParams)
{
	for(;;)
	{
		/* BLE Server Task -> After some interval it should connect with BLE Client (Phone) and synchronize the Data */
	}
}
