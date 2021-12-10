/*
 * config.c
 *
 *  Created on: 07-Dec-2021
 *      Author: Treel
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "config.h"
#include "driver/gpio.h"


/* RTOS Stuff */
QueueHandle_t xDebugQueue;
SemaphoreHandle_t xDebugQueueMutex;

void init_gpios(void)
{
	//zero-initialize the config structure.
	gpio_config_t io_conf = {};
	//disable interrupt
	io_conf.intr_type = GPIO_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_OUTPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	//disable pull-down mode
	io_conf.pull_down_en = 0;
	//disable pull-up mode
	io_conf.pull_up_en = 0;
	//configure GPIO with the given settings
	gpio_config(&io_conf);
}
void gpio_task_example(void* pvParams)
{
	for(;;)
	{
		int cnt = 0;
		while(1) {
			printf("cnt: %d tick rate: %d\n", cnt++,portTICK_RATE_MS);
			vTaskDelay(1000 / portTICK_RATE_MS);
			gpio_set_level(USER_LED, cnt % 2);
			gpio_set_level(USER_LED, cnt % 2);
		}
	}
}

void prvDebug_Task(void* pvParams)
{
	for(;;)
	{

	}
}
