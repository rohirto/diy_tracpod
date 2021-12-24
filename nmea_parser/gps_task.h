/*
 * gps_task.h
 *
 *  Created on: 09-Dec-2021
 *      Author: Treel
 */

#ifndef GPS_TASK_H_
#define GPS_TASK_H_


/* Structs */
typedef struct
{
	bool busy;
	bool valid_data;
	float latitude;                                                /*!< Latitude (degrees) */
	float longitude;                                               /*!< Longitude (degrees) */
	float altitude;                                                /*!< Altitude (meters) */

}m_gps_handle;



/* Task definations */
void gps_task(void*);

#endif /* GPS_TASK_H_ */
