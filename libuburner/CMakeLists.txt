#-----------------------------------------------#
#	libuburner CMake configuration		#
#	Authors : Asif Mahmud Shimon		#
#	Last update : 10 June 2014		#
#-----------------------------------------------#

#-----------------------------------------------#
#       For PGM_LOAD_CODE                       #
#-----------------------------------------------#

FOREACH( PGM ${UB_PROGRAMMERS} )
SET( PGM_INCLUDES
"${PGM_INCLUDES}
#include <${PGM}.h>"
)
SET( INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/externals/programmers/${PGM}
)
SET( PGM_LOAD_CODE
    "${PGM_LOAD_CODE}
    /****** For ${PGM} ******/
    programmer_list_t * tmp_${PGM};
    MALLOC_FOR_PGM(tmp_${PGM});
    sprintf(tmp_${PGM}->pgm->name,\"%s\",${PGM}_name);
    sprintf(tmp_${PGM}->pgm->about,\"%s\",${PGM}_about);
    sprintf(tmp_${PGM}->pgm->supported_family,\"%s\",${PGM}_supported_family);
    tmp_${PGM}->pgm->read = ${PGM}_read;
    tmp_${PGM}->pgm->write = ${PGM}_write;
    tmp_${PGM}->pgm->erase = ${PGM}_erase;
    tmp_${PGM}->pgm->verify = ${PGM}_verify;
    tmp_${PGM}->pgm->register_update_cb = ${PGM}_register_update_cb;
    if(ret == NULL) ret = tmp_${PGM};
    else{
       programmer_list_t * tmp_ret = ret;
       while(tmp_ret->next){
           tmp_ret = tmp_ret->next;
       }
       tmp_ret->next = tmp_${PGM};
    }"
)
ADD_SUBDIRECTORY( ${PROJECT_SOURCE_DIR}/externals/programmers/${PGM} ${PROJECT_BINARY_DIR}/externals/programmers/${PGM} )
SET( PGM_LIBS
    ${PGM_LIBS}
    ${PROJECT_BINARY_DIR}/lib/lib${PGM}.a
)
ENDFOREACH( PGM )

#-----------------------------------------------#
#	Include Directories			#
#-----------------------------------------------#
SET( INCLUDE_DIRS 
        ${INCLUDE_DIRS}
	include 
	include/hidapi
	include/libuburner
	include/sxmlc
)

#-----------------------------------------------#
#	Include Directories			#
#	OS Specific				#
#-----------------------------------------------#
if( WIN32 )
SET( INCLUDE_DIRS
	${INCLUDE_DIRS}
	include/dlfcn_win32
 )
endif()

#-----------------------------------------------#
#	Headers					#
#-----------------------------------------------#
SET( HEADERS
	include/hidapi/hidapi.h
	include/sxmlc/sxmlc.h
	include/sxmlc/sxmlsearch.h
	include/sxmlc/sxmlutils.h
	include/libuburner/opprog.h 
	include/libuburner/common.h
	include/libuburner/plugin.h
	include/libuburner/plugin_loader.h
	include/libuburner/programmer.h
	include/libuburner/programmer_loader.h
	include/libuburner/mcu_def.h
	include/libuburner/mcu_loader.h
	include/libuburner/file_def.h
	include/libuburner/file_util.h
	include/libuburner/util.h
	include/libuburner/instructions.h
)

#-----------------------------------------------#
#	Headers					#
#	OS Specific				#
#-----------------------------------------------#
if( WIN32 )
SET( HEADERS
	${HEADERS}
	include/dlfcn_win32/dlfcn.h
)
endif()

#-----------------------------------------------#
#	Configuration file			#
#-----------------------------------------------#
CONFIGURE_FILE( include/libuburner/config.h.in ${PROJECT_BINARY_DIR}/libuburner/include/libuburner/config.h )
CONFIGURE_FILE( src/libuburner/programmer_loader.c.in ${PROJECT_BINARY_DIR}/libuburner/src/libuburner/programmer_loader.c )
SET( HEADERS  
	${HEADERS}
	${PROJECT_BINARY_DIR}/libuburner/include/libuburner/config.h
)
SET( INCLUDE_DIRS 
	${INCLUDE_DIRS}
	${PROJECT_BINARY_DIR}/libuburner/include/libuburner
)
SET( SRC
        ${SRC}
        ${PROJECT_BINARY_DIR}/libuburner/src/libuburner/programmer_loader.c
)
#-----------------------------------------------#
#	Source Files				#
# 	Platform Specific			#
#-----------------------------------------------#
if( WIN32 )
SET( SRC
        ${SRC}
 	src/hidapi/windows/hid.c
	src/dlfcn_win32/dlfcn.c
)
elseif( APPLE )
SET( SRC
        ${SRC}
	src/hidapi/mac/hid.c
 )

else()
SET( SRC
        ${SRC}
	src/hidapi/unix/hid.c
 )
endif()

#-----------------------------------------------#
#	Source Files 				#
#	Platform Independent			#
#-----------------------------------------------#
SET( SRC
	${SRC}
	src/libuburner/opprog.c
	src/libuburner/util.c
	src/libuburner/file_util.c
	src/libuburner/mcu_loader.c
	src/libuburner/plugin_loader.c
    src/libuburner/programmer.c
	src/sxmlc/sxmlc.c
	src/sxmlc/sxmlsearch.c
	src/sxmlc/sxmlutils.c
 )

#-----------------------------------------------#
#	Find Libraries				#
#	Platform specific			#
#-----------------------------------------------#
MESSAGE( "=== Looking for necessary libraries ..." )
if( WIN32 )
SET( SETUP_API setupapi )
SET( LIBS
	${SETUP_API}
)
elseif( APPLE )
FIND_LIBRARY( IOKIT IOKit REQUIRED )
MESSAGE( "===> Found IOKit as : ${IOKIT}" )
FIND_LIBRARY( CORE_FOUNDATION CoreFoundation REQUIRED )
MESSAGE( "===> Found CoreFoundatin as : ${CORE_FOUNDATION}" )
FIND_LIBRARY( DLFCN dl REQUIRED )
MESSAGE( "===> Found dlfcn as : ${DLFCN}" )
SET( LIBS
	${IOKIT} 
	${CORE_FOUNDATION}
	${DLFCN}
)
else()
FIND_LIBRARY( UDEV udev REQUIRED )
MESSAGE( "===> Found udev as : ${UDEV}" )
FIND_LIBRARY( DLFCN dl REQUIRED )
MESSAGE( "===> Found dlfcn as : ${DLFCN}" )
SET( LIBS
 	${UDEV}
	${DLFCN}
)
endif()

#-----------------------------------------------#
#	Build library				#
#-----------------------------------------------#
ADD_DEFINITIONS( -Wno-format-security )
INCLUDE_DIRECTORIES( ${INCLUDE_DIRS} )
ADD_LIBRARY( uburner STATIC ${SRC} ${HEADERS} )
TARGET_LINK_LIBRARIES( uburner ${LIBS} ${PGM_LIBS} )
# Move the built library in bin folder
ADD_CUSTOM_COMMAND( TARGET uburner
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	$<TARGET_FILE:uburner> ${PROJECT_BINARY_DIR}/lib/$<TARGET_FILE_NAME:uburner> )
