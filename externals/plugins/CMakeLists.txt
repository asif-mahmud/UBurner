INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/libuburner/include
	${PROJECT_SOURCE_DIR}/libuburner/include/libuburner
	${PROJECT_SOURCE_DIR}/libuburner/include/hidapi
)
SET(CMAKE_SHARED_MODULE_PREFIX)

##Plugins Here
ADD_LIBRARY( create_mcu_conf MODULE  create_mcu_conf.c )
TARGET_LINK_LIBRARIES( create_mcu_conf ${LIB_OPPROG} )
ADD_CUSTOM_COMMAND( TARGET create_mcu_conf POST_BUILD  COMMAND ${CMAKE_COMMAND} -E rename
	$<TARGET_FILE:create_mcu_conf> ${PROJECT_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:create_mcu_conf>
)
