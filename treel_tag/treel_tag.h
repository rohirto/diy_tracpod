/*
 * treel_tag.h
 *
 *  Created on: Dec 31, 2021
 *      Author: rohirto
 */

#ifndef TREEL_TAG_H_
#define TREEL_TAG_H_

#include <stdio.h>
#include <stdint.h>

#include "esp_bt_defs.h"
#include "app_config.h"

//DEfines
#define TAG_FILE_TYPE		0x02
#define FRONT_TAG_TYPE		0x01
#define REAR_TAG_TYPE		0x02
#define TREEL_ADV_DATA_LEN	31
//Structs

typedef struct {
	uint16_t tag_temperature;
	uint16_t tag_pressure;
	uint16_t tag_Xvibration;
	uint16_t tag_Zvibration;
	uint16_t tag_FWversion;
	uint8_t tag_battery;
	uint8_t	tag_eventFlags;
	uint8_t tag_tamperCount;
	uint8_t tag_Xoffset;
	uint8_t tag_Zoffset;

	esp_bd_addr_t tag_addr;
	bool tag_detected;
	uint8_t payload_buff[TREEL_ADV_DATA_LEN];
}treel_tag_data;


typedef struct
{
	uint8_t	tag_type;		/* 0x01 Front , 0x02 Rear*/
	esp_bd_addr_t tag_addr;
	uint16_t tag_temperature;
	uint16_t tag_pressure;
	uint8_t tag_battery;

}tag_queue_msg;

//Function Prototypes
app_ret filter_data(treel_tag_data*);
app_ret process_data(treel_tag_data*);
app_ret print_tag_data(treel_tag_data*);
void decrypt(unsigned char *, char * , unsigned char * );

#endif /* TREEL_TAG_H_ */
