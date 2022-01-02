/*
 * config.h
 *
 *  Created on: 07-Dec-2021
 *      Author: Treel
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/* Includes */
#include <string.h>
#include <stdio.h>

/* Board Config */
#define USER_LED	2
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<USER_LED))
#define uS_TO_S_FACTOR 		1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  		5        //Time ESP32 will go to sleep (in seconds)
#define ms_TO_S_FACTOR		1000
#define sec_TO_mins_FACTOR	60

//Defines
#define TICKS_TO_WAIT  	10
//#define GPS_LOG		("GPS.txt")
/* Enum */
typedef enum
{
	app_success,
	app_error
}app_ret;

/* RTOS Stuff - tasks and Queues */

/* Func Prototypes */
void init_gpios(void);
void gpio_task_example(void*);
void prvDebug_Task(void*);
void init_task(void*);


#endif /* APP_CONFIG_H_ */
