set(includedirs
	../config
	../nmea_parser
	../ble_tasks
	../flash
	../sdcard_driver
	../treel_tag
	.)

set(src
	gattc_gatts_coex.c
	../config/app_config.c
	../nmea_parser/nmea_parser.c
	../nmea_parser/gps_task.c
	../flash/flash_task.c
	../sdcard_driver/sdcard.c
	../treel_tag/treel_tag.c
	../ble_tasks/ble_client.c
	../ble_tasks/ble_server.c)

idf_component_register(SRCS ${src}
                    INCLUDE_DIRS  ${includedirs})
