/*
 * config.h
 *
 *  Created on: 07-Dec-2021
 *      Author: Treel
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Board Config */
#define USER_LED	2
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<USER_LED))

/* Func Prototypes */
void init_gpios(void);
void gpio_task_example(void*);


#endif /* CONFIG_H_ */
