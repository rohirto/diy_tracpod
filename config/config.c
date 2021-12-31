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
#include "nmea_parser.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdcard.h"


static const char *SLEEP_TAG = "SLEEP";
static const char *DEBUG_TAG = "DEBUG";
char ptrTaskList[250];


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

/* Init of Application level stuff */
void init_task(void* pvParams)
{
	for(;;)
	{
		//Create or open the SD card File
		create_file("log_file.txt");

		//Delete the Task
		vTaskDelete(NULL);

	}
}

void prvDebug_Task(void* pvParams)
{

	//gps_t *gps = NULL;

	for(;;)
	{
		vTaskList(ptrTaskList);
		ESP_LOGI(DEBUG_TAG,"**********************************");
		ESP_LOGI(DEBUG_TAG,"Task  State   Prio    Stack    Num");
		ESP_LOGI(DEBUG_TAG,"**********************************");
		ESP_LOGI(DEBUG_TAG, "%s",(char*)ptrTaskList);
		ESP_LOGI(DEBUG_TAG,"**********************************");

		vTaskDelay(pdMS_TO_TICKS(10000));

	}
}

void timer_deep_sleep(int mins)
{

	printf("Enabling timer wakeup, %d mins\n", mins);
	esp_sleep_enable_timer_wakeup(mins*sec_TO_mins_FACTOR * uS_TO_S_FACTOR);
	ESP_LOGI(SLEEP_TAG,"Entering Deep Sleep");
	esp_deep_sleep_start();
}

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : ESP_LOGI(SLEEP_TAG,"Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : ESP_LOGI(SLEEP_TAG,"Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : ESP_LOGI(SLEEP_TAG,"Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : ESP_LOGI(SLEEP_TAG,"Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : ESP_LOGI(SLEEP_TAG,"Wakeup caused by ULP program"); break;
    default : ESP_LOGI(SLEEP_TAG,"Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}




