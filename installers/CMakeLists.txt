#custom installer maker for inno setup
if( WIN32 )
CONFIGURE_FILE( win32/win-installer.iss.in ${CMAKE_BINARY_DIR}/externals/resources/win-installer.iss )
FIND_PROGRAM( InnoSetupC ISCC REQUIRED )
MESSAGE( "===> Inno Setup Found : ${InnoSetupC}" )
ADD_CUSTOM_TARGET( Make_Win32_INST )
ADD_CUSTOM_TARGET( Make_Installer ALL DEPENDS Make_Win32_INST )
ADD_CUSTOM_COMMAND( TARGET Make_Win32_INST
    POST_BUILD
    COMMAND
    ${InnoSetupC} ${CMAKE_BINARY_DIR}/externals/resources/win-installer.iss
)
endif()
#-----------------------------------------------------------------------#
#		Custom uninstall script					#
#-----------------------------------------------------------------------#
# uninstall target
configure_file(
    "cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
	COMMAND 
	${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/cmake_uninstall.cmake
)
#-----------------------------------------------------------------------#

if( UNIX AND NOT APPLE )
FILE( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/linux-i386_installer )
CONFIGURE_FILE( linux-i386/installer.in ${CMAKE_BINARY_DIR}/installers/installer.sh )
FILE( COPY ${CMAKE_BINARY_DIR}/installers/installer.sh
	DESTINATION ${CMAKE_BINARY_DIR}
	FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
	GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
CONFIGURE_FILE( linux-i386/decompress.in ${CMAKE_BINARY_DIR}/installers/decompress.sh )
FILE( COPY ${CMAKE_BINARY_DIR}/installers/decompress.sh
	DESTINATION ${CMAKE_BINARY_DIR}/linux-i386_installer
	FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
	GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_CUSTOM_TARGET( Linux_INST_MAKER )
ADD_CUSTOM_TARGET( Linux_INST_MAKER_TGT ALL DEPENDS Linux_INST_MAKER )

ADD_CUSTOM_COMMAND( TARGET Linux_INST_MAKER
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMAND
	tar -cf linux-i386_installer/payload.tar bin lib32 mcus plugins docs resources installer.sh
)
ADD_CUSTOM_COMMAND( TARGET Linux_INST_MAKER
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/linux-i386_installer
	COMMAND
	gzip payload.tar
)
ADD_CUSTOM_COMMAND( TARGET Linux_INST_MAKER
	POST_BUILD
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/linux-i386_installer
	COMMAND
	cat decompress.sh payload.tar.gz > ${PROJECT_NAME}-${UBurner_PACKAGE_VERSION}-i386-installer.sh
)
endif()

