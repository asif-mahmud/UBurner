SET( MCUS
    atmega8
    pic18f2550
)
ADD_CUSTOM_TARGET( Copy_MCU_Files )
ADD_CUSTOM_TARGET( Copy_All_MCUS ALL DEPENDS Copy_MCU_Files )
FOREACH( MCU ${MCUS} )
ADD_CUSTOM_COMMAND( TARGET Copy_MCU_Files
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/externals/mcus/${MCU}.xml ${PROJECT_BINARY_DIR}/mcus
)
ENDFOREACH()
