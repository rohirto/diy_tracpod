/*
 * ble_tasks.h
 *
 *  Created on: 09-Dec-2021
 *      Author: Treel
 */

#ifndef BLE_TASKS_H_
#define BLE_TASKS_H_

#include "esp_gatt_defs.h"
#include "app_config.h"
#include <stdio.h>
#include <stdint.h>

//Defines
/* GPS Defines */
#define DEVICE_INFO_SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define DEVICE_INFO_CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define GPS_DATA_SERVICE_UUID             "5271bfd3-4b5f-4397-84f6-f02678d91881"
#define LOCATION_DATA_CHARACTERISTIC_UUID "173c1d4e-b2a3-4452-bf2e-b62c43f17796"
#define DATE_DATA_CHARACTERISTIC_UUID     "d5ae635f-9aeb-4ec1-abae-526687c2d350"
#define TIME_DATA_CHARACTERISTIC_UUID     "2282aec6-ffe7-423c-b6b4-e854bbba997f"
#define SPEED_DATA_CHARACTERISTIC_UUID    "97c7bc02-0b24-4119-872b-35eece505c7c"
#define ALTITUDE_DATA_CHARACTERISTIC_UUID "f578b138-0076-42d8-9111-06101a1a2981"
/* TPMS Defines */
#define TREEL_SERVICE_UUID    "0000ffe0-0000-1000-8000-00805f9b34fb"
/* Timer to Sleep */
#define APP_BLE_TIMEOUT	2		//Time ESP32 will go to sleep (in mins)
#define APP_GPS_TIMEOUT	2		//Time ESP32 will go to sleep (in mins)
#define APP_BLE_SCAN_INTERVAL			//in ms
#define APP_BLE_SCAN_TIME		10		//in secs

#define TAG_BUFFER_LEN			17
#define GPS_CURR_FILE			0x01
#define FRONT_CURR_FILE			0x02
#define REAR_CURR_FILE			0x03
#define NO_CURR_FILE			0x04


//Structs
typedef struct
{
	bool connected;
	bool busy;
	bool notify_enabled;
	esp_gatt_if_t app_gatt_if;
	uint16_t conn_id;
	uint16_t char_handle;
	size_t data_size;
	uint8_t notify_data[38];
	uint8_t current_file;		/* 0x01 GPS File, 0x02 Front File, 0x03 Rear File, 0x04 No file */


}ble_server_handle;

typedef struct
{
	bool scan_complete;
}ble_client_handle;


/* BLE Tasks definitions */
void ble_client_task(void*);
void ble_server_task(void*);
app_ret gps_file_handling();
app_ret f_tag_file_handling();
app_ret r_tag_file_handling();



#endif /* BLE_TASKS_H_ */
