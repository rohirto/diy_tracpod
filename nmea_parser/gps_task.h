/*
 * gps_task.h
 *
 *  Created on: 09-Dec-2021
 *      Author: Treel
 */

#ifndef GPS_TASK_H_
#define GPS_TASK_H_


//defines
#define GPS_BUFFER_LEN	38
#define GPS_FILE_TYPE 0x01

/* Structs */
typedef struct
{
	bool busy;
	bool valid_data;
	bool init;
}m_gps_handle;



/* Task definations */
void gps_task(void*);

#endif /* GPS_TASK_H_ */
