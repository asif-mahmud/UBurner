#-------------------------------#
#	Find QT 		#
#-------------------------------#
FIND_PACKAGE( Qt4 REQUIRED )
MESSAGE( "===> Qt4 Found in : ${QT_USE_FILE}" )

INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS( ${QT_DEFINITIONS} )


#-------------------------------#
#	Configuration		#
#-------------------------------#
SET( CONF_WINDOW_NAME "UBurner" )
SET( CONF_WINDOW_TITLE "${CONF_WINDOW_NAME}-v${UBurner_PACKAGE_VERSION}(${UBurner_AUTHORS})" )
SET( CONF_GO_ICON	"../resources/go.png" )
SET( CONF_ERASE_ICON "../resources/erase.png" )
SET( CONF_GREEN_ICON "../resources/green.png" )
SET( CONF_RED_ICON "../resources/red.png" )

CONFIGURE_FILE( include/gui/gui_config.hpp.in ${PROJECT_BINARY_DIR}/qt_gui/include/gui/gui_config.hpp )

SET( INCLUDE_DIRS  ${PROJECT_BINARY_DIR}/qt_gui/include/gui)

#-------------------------------#
#	Include Directories	#
#-------------------------------#
SET( INCLUDE_DIRS
	${INCLUDE_DIRS}
	include/uburner
	include/gui
)

SET( UB_INCLUDE_DIRS
	${PROJECT_SOURCE_DIR}/libuburner/include/libuburner
	${PROJECT_SOURCE_DIR}/libuburner/include/hidapi
)

if( WIN32 )
SET( UB_INCLUDE_DIRS ${UB_INCLUDE_DIRS} 
	${PROJECT_SOURCE_DIR}/libuburner/include/dlfcn_win32
)
endif()

INCLUDE_DIRECTORIES( ${INCLUDE_DIRS} ${UB_INCLUDE_DIRS} )

#-------------------------------#
#	CPP sources		#
#	NOT QT_MOC ONES		#
#-------------------------------#
SET( cpp_SRC
	src/main.cpp
	src/gui/splash.cpp
	src/gui/about_mcu_widget.cpp
	src/gui/mcu_viewer.cpp
	src/gui/action_widget.cpp
	src/gui/config_widget.cpp
	src/gui/main_win.cpp
	src/gui/action_thread.cpp
	src/gui/about_widget.cpp
	src/gui/global_qt_gui.cpp
	src/uburner/opprog.cpp
)

SET( other_HEADERS
	include/uburner/opprog_int.hpp
	include/gui/global_qt_gui.hpp
)

#-------------------------------#
#	QT_MOC Headers		#
#-------------------------------#
SET( moc_HEADERS
	include/gui/splash.hpp
	include/gui/action_thread.hpp
	include/gui/mcu_viewer.hpp
	include/gui/about_mcu_widget.hpp
	include/gui/config_widget.hpp
	include/gui/about_widget.hpp
	include/gui/action_widget.hpp
	include/gui/main_win.hpp
)

QT4_WRAP_CPP( moc_CPP_SRC ${moc_HEADERS} )


#-------------------------------#
#	Add Executable		#
#	Platform independent	#
#-------------------------------#
if( "${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE" OR "${CMAKE_BUILD_TYPE}" STREQUAL "release" AND WIN32 )
SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows" )
endif()
ADD_EXECUTABLE( UBurner
		${cpp_SRC}
		${moc_CPP_SRC}
		${moc_HEADERS}
		${other_HEADERS}
)
if( WIN32 )
TARGET_LINK_LIBRARIES( UBurner ${QT_LIBRARIES} ${LIB_UBURNER} ${RC_OBJ} )
elseif(UNIX AND NOT APPLE)
TARGET_LINK_LIBRARIES( UBurner ${QT_LIBRARIES} ${LIB_UBURNER} )
endif()

#Move the executable in correct directory
ADD_CUSTOM_COMMAND( TARGET UBurner 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:UBurner> ${PROJECT_BINARY_DIR}/bin/$<TARGET_FILE_NAME:UBurner>
)
