INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/libuburner/include
	${PROJECT_SOURCE_DIR}/libuburner/include/libuburner
	${PROJECT_SOURCE_DIR}/libuburner/include/hidapi
	.
)

ADD_LIBRARY( atmel STATIC atmel.c at_read.c at_write.c at_verify.c at_erase.c )
ADD_CUSTOM_COMMAND( TARGET atmel POST_BUILD  COMMAND ${CMAKE_COMMAND} -E rename
	$<TARGET_FILE:atmel> ${PROJECT_BINARY_DIR}/lib/$<TARGET_FILE_NAME:atmel>
)
