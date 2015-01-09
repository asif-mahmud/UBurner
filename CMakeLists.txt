CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( UBurner )

#-----------------------------------------#
#		Version Management 	  #
#-----------------------------------------#
SET( UBurner_VERSION_MAJOR 0 )
SET( UBurner_VERSION_MINOR 1 )
SET( UBurner_VERSION_REV   2 )

SET( UBurner_PACKAGE_VERSION 
        "${UBurner_VERSION_MAJOR}.${UBurner_VERSION_MINOR}.0.${UBurner_VERSION_REV}" )

##Debug and Release
if( CMAKE_BUILD_TYPE MATCHES DEBUG OR CMAKE_BUILD_TYPE MATCHES debug )
MESSAGE( "===> BUILD TYPE : ${CMAKE_BUILD_TYPE}" )
SET( CMAKE_VERBOSE_MAKEFILE ON )
endif()
SET( CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -std=gnu99" )
SET( CMAKE_SKIP_RPATH ON )
SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PROJECT_BINARY_DIR}/lib " )
if( UNIX )
SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,../lib32 " )
endif()
if( WIN32 )
SET( RC_OBJ "${PROJECT_BINARY_DIR}/externals/resources/resources.obj" )
SET( CMDRC_OBJ "${PROJECT_BINARY_DIR}/externals/resources/cmd.obj" )
endif()

#----------------------------------------#
#		URL			 #
#----------------------------------------#
SET( UBurner_PACKAGE_URL "http://www.absurdme.com/uburner/" )
SET( UBurner_PACKAGE_CONTACT "uburner@absurdme.com" )


#----------------------------------------#
#	Authors				 #
#----------------------------------------#
SET( UBurner_AUTHORS "Asif Mahmud Shimon" )

#----------------------------------------#
#   Programmers  by name                 #
#----------------------------------------#
SET( UB_PROGRAMMERS
    atmel
    pic18f
)


SET( UB_PROGRAMMERS_CNT
    0
)
FOREACH( PGM ${UB_PROGRAMMERS} )
    SET( UB_PGM_LIST ${UB_PGM_LIST} ${PROJECT_BINARY_DIR}/lib/lib${PGM}.a )
    MATH( EXPR UB_PROGRAMMERS_CNT "${UB_PROGRAMMERS_CNT}+1" )
    SET( CONF_PGM_LIST "${CONF_PGM_LIST}
                        \"${PGM}\"," )
ENDFOREACH( PGM )
MESSAGE( "===> PGM List : ${UB_PGM_LIST}" )
MESSAGE( "===> PGM Count : ${UB_PROGRAMMERS_CNT}" )

#----------------------------------------#
#	Configurations			 #
#	OS Specific			 #
#----------------------------------------#
SET( CONF_WIN_ICON "../resources/uburner.png" )
SET( CONF_SPLASH_SCREEN "../resources/splash.png" )
SET( CONF_RESOURCE_DIR "../resources" )
SET( CONF_MCUS_DIR "../mcus" )
SET( CONF_PLUGINS_DIR "../plugins" )
SET( CONF_DOCS_DIR "../docs" )
SET( CONF_DEFAULT_STYLE  "../resources/default.style" )

#----------------------------------------#
#	Set Plugin Suffix		 #
#	it can be .so/.dll/.dylib	 #
#----------------------------------------#
if( APPLE )
SET( CONF_PLUGIN_SUFFIX	 ".dylib" )
elseif( WIN32 )
SET( CONF_PLUGIN_SUFFIX	 ".dll" )
else()
SET( CONF_PLUGIN_SUFFIX	 ".so" )
endif()

#-------------------------------#
#	Link against 		#
#	libuburner		#
#-------------------------------#
if( APPLE )
SET( LIB_UBURNER ${PROJECT_BINARY_DIR}/lib/libuburner.a ${UB_PGM_LIST} )
elseif( WIN32 )
SET( LIB_UBURNER ${PROJECT_BINARY_DIR}/lib/libuburner.a ${UB_PGM_LIST} setupapi )
else()
SET( LIB_UBURNER ${PROJECT_BINARY_DIR}/lib/libuburner.a ${UB_PGM_LIST} dl udev )
endif()

#----------------------------------------#
#	Make necessary directories	 #
#----------------------------------------#
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/plugins )
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/mcus )
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/docs )
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/resources )

#----------------------------------------#
#		Copy and rpath linux libs		 #
#----------------------------------------#
if( UNIX AND NOT APPLE )
FILE( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/lib32 )
FIND_PROGRAM( PATCHELF patchelf REQUIRED )

SET( LINUX_DEPS
	/opt/qt-4.8.6/lib/libQtCore.so.4
	/opt/qt-4.8.6/lib/libQtGui.so.4
	/lib/i386-linux-gnu/libdl.so.2
	/lib/i386-linux-gnu/libudev.so.1
	/usr/lib/i386-linux-gnu/libstdc++.so.6
	/lib/i386-linux-gnu/libgcc_s.so.1
	/lib/i386-linux-gnu/libc.so.6
	/lib/i386-linux-gnu/libpthread.so.0
	/lib/i386-linux-gnu/libglib-2.0.so.0
	/usr/lib/i386-linux-gnu/libfreetype.so.6
	/usr/lib/i386-linux-gnu/libgobject-2.0.so.0
	/usr/lib/i386-linux-gnu/libSM.so.6
	/usr/lib/i386-linux-gnu/libICE.so.6
	/usr/lib/i386-linux-gnu/libXrender.so.1
	/usr/lib/i386-linux-gnu/libfontconfig.so.1
	/usr/lib/i386-linux-gnu/libXext.so.6
	/usr/lib/i386-linux-gnu/libX11.so.6
	/lib/i386-linux-gnu/libm.so.6
	/lib/i386-linux-gnu/librt.so.1
	/lib/i386-linux-gnu/libcgmanager.so.0
	/lib/i386-linux-gnu/libnih.so.1
	/lib/i386-linux-gnu/libnih-dbus.so.1
	/lib/i386-linux-gnu/libdbus-1.so.3
	/lib/i386-linux-gnu/libpcre.so.3
	/lib/i386-linux-gnu/libz.so.1
	/lib/i386-linux-gnu/libpng12.so.0
	/usr/lib/i386-linux-gnu/libffi.so.6
	/lib/i386-linux-gnu/libuuid.so.1
	/lib/i386-linux-gnu/libexpat.so.1
	/usr/lib/i386-linux-gnu/libxcb.so.1
	/usr/lib/i386-linux-gnu/libXau.so.6
	/usr/lib/i386-linux-gnu/libXdmcp.so.6
)

SET( LINUX_DEP_NAMES
	libQtCore.so.4
	libQtGui.so.4
	libdl.so.2
	libudev.so.1
	libstdc++.so.6
	libgcc_s.so.1
	libc.so.6
	libpthread.so.0
	libglib-2.0.so.0
	libfreetype.so.6
	libgobject-2.0.so.0
	libSM.so.6
	libICE.so.6
	libXrender.so.1
	libfontconfig.so.1
	libXext.so.6
	libX11.so.6
	libm.so.6
	librt.so.1
	libcgmanager.so.0
	libnih.so.1
	libnih-dbus.so.1
	libdbus-1.so.3
	libpcre.so.3
	libz.so.1
	libpng12.so.0
	libffi.so.6
	libuuid.so.1
	libexpat.so.1
	libxcb.so.1
	libXau.so.6
	libXdmcp.so.6
)

ADD_CUSTOM_TARGET( Copy_LD_Files )
ADD_CUSTOM_TARGET( Copy_All_LD ALL DEPENDS Copy_LD_Files )
FOREACH( LIB ${LINUX_DEPS} ) #copy all
ADD_CUSTOM_COMMAND( TARGET Copy_LD_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${LIB} ${PROJECT_BINARY_DIR}/lib32
)
ENDFOREACH()
FOREACH( LIB ${LINUX_DEP_NAMES} ) # patch rpath
ADD_CUSTOM_COMMAND( TARGET Copy_LD_Files
	PRE_BUILD
	COMMAND
	${PATCHELF} --set-rpath . ${PROJECT_BINARY_DIR}/lib32/${LIB}
)
SET( UB_LINUX_LINK_LIBS 
	${UB_LINUX_LINK_LIBS}
	${PROJECT_BINARY_DIR}/lib32/${LIB}
)
ENDFOREACH()

endif()

#----------------------------------------#
#		Subdirectories		 #
#----------------------------------------#
ADD_SUBDIRECTORY( libuburner )
ADD_SUBDIRECTORY( externals )
#ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( qt_gui )
ADD_SUBDIRECTORY( installers )
