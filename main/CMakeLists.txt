set(includedirs
	../gps
	../config
	.)

set(src
	gattc_gatts_coex.c
	../config/config.c
	../gps/gps_driver.c)

idf_component_register(SRCS ${src}
                    INCLUDE_DIRS  ${includedirs})
