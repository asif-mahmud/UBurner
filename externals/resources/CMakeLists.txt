SET( CONF_DOWN_ARROW "../resources/down_arrow.png" )
SET( CONF_HANDLE "../resources/handle.png" )
SET( CONF_CHECKBOX "../resources/checkbox.png" )

# Just Copy the images in resources folder
ADD_CUSTOM_TARGET( Copy_Files )
ADD_CUSTOM_TARGET( Copy_All ALL DEPENDS Copy_Files )

ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/splash.png ${PROJECT_BINARY_DIR}/resources
)

ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/uburner.png ${PROJECT_BINARY_DIR}/resources
)

ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/uburner.ico ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/cmd.ico ${PROJECT_BINARY_DIR}/resources
)
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/externals/resources/default.style.in" "${PROJECT_BINARY_DIR}/resources/default.style" )
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/checkbox.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/down_arrow.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/handle.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/go.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/green.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/red.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/LICENSE ${PROJECT_BINARY_DIR}/docs
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_BINARY_DIR}/docs
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/erase.png ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
	PRE_BUILD
	COMMAND
	${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/installer.bmp ${PROJECT_BINARY_DIR}/resources
)
ADD_CUSTOM_COMMAND( TARGET Copy_Files
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/resources/installer_small.bmp ${PROJECT_BINARY_DIR}/resources
)
if( WIN32 )
	SET( RC_COMPILER windres )
	SET( RC_SOURCE resources.rc )
	SET( CMDRC_SOURCE cmd.rc )
	SET( RC_OBJECT resources.obj )
	SET( CMDRC_OBJECT cmd.obj )
	CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/externals/resources/${RC_SOURCE}.in" "${PROJECT_BINARY_DIR}/externals/resources/${RC_SOURCE}" )
	CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/externals/resources/UBurner.exe.manifest.in" "${PROJECT_BINARY_DIR}/externals/resources/UBurner.exe.manifest" )
	CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/externals/resources/${CMDRC_SOURCE}.in" "${PROJECT_BINARY_DIR}/externals/resources/${CMDRC_SOURCE}" )
	ADD_CUSTOM_COMMAND( TARGET Copy_Files
		PRE_BUILD
		COMMAND ${RC_COMPILER} -i${PROJECT_BINARY_DIR}/externals/resources/${RC_SOURCE} -o ${PROJECT_BINARY_DIR}/externals/resources/${RC_OBJECT} --output-format=coff 
	)
	ADD_CUSTOM_COMMAND( TARGET Copy_Files
		PRE_BUILD
		COMMAND ${RC_COMPILER} -i${PROJECT_BINARY_DIR}/externals/resources/${CMDRC_SOURCE} -o ${PROJECT_BINARY_DIR}/externals/resources/${CMDRC_OBJECT} --output-format=coff 
	)
       
        FIND_LIBRARY( MINGWM10 mingwm10 REQUIRED )
        MESSAGE( "===> mingwm10 Found as ${MINGWM10}" )
        FIND_LIBRARY( QTCORE4 QtCore4 REQUIRED )
        MESSAGE( "===> QtCore4 Found as ${QTCORE4}" )
        FIND_LIBRARY( QTGUI4 QtGui4 REQUIRED )
        MESSAGE( "===> QtGui4 Found as ${QTGUI4}" )
        FIND_LIBRARY( GCC_S_DW2_1 gcc_s_dw2-1 REQUIRED )
        MESSAGE( "===> gcc_s_dw2-1 Found as ${GCC_S_DW2_1}" )
        ADD_CUSTOM_COMMAND( TARGET Copy_Files
                PRE_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy ${GCC_S_DW2_1} ${PROJECT_BINARY_DIR}/bin
        )
        ADD_CUSTOM_COMMAND( TARGET Copy_Files
                PRE_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy ${QTCORE4} ${PROJECT_BINARY_DIR}/bin
        )
        ADD_CUSTOM_COMMAND( TARGET Copy_Files
                PRE_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy ${QTGUI4} ${PROJECT_BINARY_DIR}/bin
        )
         ADD_CUSTOM_COMMAND( TARGET Copy_Files
                PRE_BUILD
                COMMAND
                ${CMAKE_COMMAND} -E copy ${MINGWM10} ${PROJECT_BINARY_DIR}/bin
        )
endif()
