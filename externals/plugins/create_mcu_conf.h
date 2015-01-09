#ifndef CREATE_MCU_CONF_H
#define CREATE_MCU_CONF_H

#include <plugin.h>
#include <common.h>
#include <mcu_def.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

void create_mcu_conf_init( plugin_t * );
void create_mcu_conf_run(opprog_t *, mcu_info_t *,char *, int);

#endif //CREATE_MCU_CONF_H
