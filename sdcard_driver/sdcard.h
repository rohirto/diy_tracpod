/*
 * sdcard.h
 *
 *  Created on: 24-Dec-2021
 *      Author: Treel
 */

#ifndef SDCARD_H_
#define SDCARD_H_

#include "config.h"

//Structs
typedef struct
{
	bool mounted;
	bool busy;

}sdcard_handle;

//Function Prototypes
void sdcard_Task(void* );
app_ret sdcard_init(void);
void sd_handle_init(void);
app_ret delete_file(char* );
app_ret write_data_to_file(char* , char* );
app_ret read_data_from_file(char* , char* , size_t );
app_ret rename_file(char* , char* );
app_ret sdcard_deinit(void);

#endif /* SDCARD_H_ */
