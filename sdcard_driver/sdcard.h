/*
 * sdcard.h
 *
 *  Created on: 24-Dec-2021
 *      Author: Treel
 */

#ifndef SDCARD_H_
#define SDCARD_H_

#include "app_config.h"
#include "esp_vfs_fat.h"

//Structs
typedef struct
{
	bool mounted;
	bool busy;

}sdcard_handle;

typedef struct
{
	uint8_t file_type; /* 0x01 GPS FILE,  0x02 TAG FILE*/
	int current_line;
	int total_lines;
	bool file_read;
	bool if_exist;
	bool valid_data;
}file_handle;

//Macros
#define SDSPI_HOST_APP() {\
	.flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG, \
	.slot = VSPI_HOST, \
	.max_freq_khz = SDMMC_FREQ_PROBING, \
	.io_voltage = 3.3f, \
	.init = &sdspi_host_init, \
	.set_bus_width = NULL, \
	.get_bus_width = NULL, \
	.set_bus_ddr_mode = NULL, \
	.set_card_clk = &sdspi_host_set_card_clk, \
	.do_transaction = &sdspi_host_do_transaction, \
	.deinit_p = &sdspi_host_remove_device, \
	.io_int_enable = &sdspi_host_io_int_enable, \
	.io_int_wait = &sdspi_host_io_int_wait, \
	.command_timeout_ms = 0, \
}

#define SDSPI_DEVICE_CONFIG_APP() {\
    .host_id   = VSPI_HOST, \
    .gpio_cs   = GPIO_NUM_5, \
    .gpio_cd   = SDSPI_SLOT_NO_CD, \
    .gpio_wp   = SDSPI_SLOT_NO_WP, \
    .gpio_int  = GPIO_NUM_NC, \
}

//Function Prototypes
void sdcard_Task(void* );
app_ret sdcard_init(void);
void sd_handle_init(void);
app_ret delete_file(char* );
app_ret write_data_to_file(char* , char* );
app_ret read_data_from_file(char* , char* , size_t );
app_ret rename_file(char* , char* );
app_ret sdcard_deinit(void);
app_ret create_file(char* );
app_ret read_line(char* ,file_handle* , char*, size_t );
app_ret get_no_lines_to_tx(char* , file_handle* );
app_ret if_exists(char* );
#endif /* SDCARD_H_ */
