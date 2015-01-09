INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/libuburner/include
	${PROJECT_SOURCE_DIR}/libuburner/include/libuburner
	${PROJECT_SOURCE_DIR}/libuburner/include/hidapi
	.
)

ADD_LIBRARY( pic18f STATIC pic18f.c pic18f_read.c pic18f_write.c pic18f_verify.c pic18f_erase.c )
ADD_CUSTOM_COMMAND( TARGET pic18f POST_BUILD  COMMAND ${CMAKE_COMMAND} -E rename
	$<TARGET_FILE:pic18f> ${PROJECT_BINARY_DIR}/lib/$<TARGET_FILE_NAME:pic18f>
)
