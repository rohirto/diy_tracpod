/*
 * gps_driver.h
 *
 *  Created on: 07-Dec-2021
 *      Author: Treel
 */

#ifndef GPS_DRIVER_H_
#define GPS_DRIVER_H_

/* Defines */
#define GPS_TAG				"GPS"
#define const_GPS_Init		"\n ==========================GPS Init Done==================================="


/* Function Prototypes */
void init_gps(void);
void blink_task(void *pvParameter);
#endif /* GPS_DRIVER_H_ */
