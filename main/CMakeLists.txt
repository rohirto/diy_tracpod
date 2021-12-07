set(includedirs
	../gps
	.)

set(src
	gattc_gatts_coex.c
	../gps/gps_driver.c)

idf_component_register(SRCS ${src}
                    INCLUDE_DIRS  ${includedirs})
