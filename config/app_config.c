/*
 * config.c
 *
 *  Created on: 07-Dec-2021
 *      Author: Treel
 */
#include "app_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "nmea_parser.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdcard.h"
#include "gps_task.h"
#include "ble_tasks.h"
#include "esp_bt_defs.h"
#include "treel_tag.h"


static const char *SLEEP_TAG = "SLEEP";
static const char *DEBUG_TAG = "DEBUG";
static const char *INIT_TAG = "INIT";
char ptrTaskList[250];
extern m_gps_handle gps_handle;
extern sdcard_handle sd_handle;
extern ble_server_handle server_handle;
extern ble_client_handle client_handle;
extern file_handle gps_file_handle;
extern file_handle f_tag_file_handle;
extern file_handle r_tag_file_handle;
extern treel_tag_data app_tag_r, app_tag_f;

void init_gpios(void)
{
	//zero-initialize the config structure.
	//USer LED
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

	/* SD Lines */
	gpio_set_pull_mode(5, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes

	gpio_set_pull_mode(18, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
	gpio_set_pull_mode(23, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
	gpio_set_pull_mode(19, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

}

void init_handles(void)
{
	int i;
	gps_handle.busy =false;
	gps_handle.init =false;
	gps_handle.valid_data = false;


	server_handle.busy = false;
	server_handle.connected = false;
	server_handle.notify_enabled = false;

	client_handle.scan_complete = false;

	gps_file_handle.current_line = 0;
	gps_file_handle.total_lines = 0;
	gps_file_handle.file_type = GPS_FILE_TYPE;
	gps_file_handle.valid_data = false;

	if(sd_handle.mounted == true)
	{
		if(if_exists("GPS.txt") == app_success)
		{
			gps_file_handle.if_exist = true;
			ESP_LOGI(INIT_TAG,"GPS File Exits!");
		}
		else
		{
			gps_file_handle.if_exist = false;
			ESP_LOGI(INIT_TAG,"GPS File Does not Exits!");
		}
	}
	else
	{
		ESP_LOGE(INIT_TAG,"SD not Mounted");
	}
	f_tag_file_handle.current_line = 0;
	f_tag_file_handle.total_lines = 0;
	f_tag_file_handle.file_type = TAG_FILE_TYPE;
	f_tag_file_handle.valid_data = false;

	if(sd_handle.mounted == true)
	{
		if(if_exists("front.txt") == app_success)
		{
			f_tag_file_handle.if_exist = true;
			ESP_LOGI(INIT_TAG,"Front Tag File Exits!");
		}
		else
		{
			f_tag_file_handle.if_exist = false;
			ESP_LOGI(INIT_TAG,"Front Tag File Does not Exits!");
		}
	}
	else
	{
		ESP_LOGE(INIT_TAG,"SD not Mounted");
	}
	r_tag_file_handle.current_line = 0;
	r_tag_file_handle.total_lines = 0;
	r_tag_file_handle.file_type = TAG_FILE_TYPE;
	r_tag_file_handle.valid_data = false;

	if(sd_handle.mounted == true)
	{
		if(if_exists("rear.txt") == app_success)
		{
			r_tag_file_handle.if_exist = true;
			ESP_LOGI(INIT_TAG,"Rear Tag File Exits!");
		}
		else
		{
			r_tag_file_handle.if_exist = false;
			ESP_LOGI(INIT_TAG,"Rear Tag File Does not Exits!");
		}
	}
	else
	{
		ESP_LOGE(INIT_TAG,"SD not Mounted");
	}



	app_tag_r.tag_detected = false;
	app_tag_f.tag_detected =false;

	memset(app_tag_r.payload_buff, '\0', TREEL_ADV_DATA_LEN);
	memset(app_tag_f.payload_buff, '\0', TREEL_ADV_DATA_LEN);

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
		//Init handles
		init_handles();

		//Create or open the SD card File
		//create_file("log_file.txt");
		//write_data_to_file("GPS.txt","GPS_Data\n");
		//write_data_to_file("Tag_log_file.txt","Tag_Data\n");
		ESP_LOGI(INIT_TAG,"Init Task completed, Deleting Task");
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




