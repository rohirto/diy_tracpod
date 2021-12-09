set(includedirs
	../config
	../nmea_parser
	../ble_tasks
	.)

set(src
	gattc_gatts_coex.c
	../config/config.c
	../nmea_parser/nmea_parser.c
	../nmea_parser/gps_task.c
	../ble_tasks/ble_client.c
	../ble_tasks/ble_server.c)

idf_component_register(SRCS ${src}
                    INCLUDE_DIRS  ${includedirs})
