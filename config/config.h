/*
 * config.h
 *
 *  Created on: 07-Dec-2021
 *      Author: Treel
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Includes */

/* Board Config */
#define USER_LED	2
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<USER_LED))





/* RTOS Stuff - tasks and Queues */

/* Func Prototypes */
void init_gpios(void);
void gpio_task_example(void*);
void prvDebug_Task(void*);


#endif /* CONFIG_H_ */
