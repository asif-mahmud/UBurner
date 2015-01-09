/**
* atmel.h
* To program/read/verify/erase your MCU in High Voltage(11.5-12.5V)
* Your mcu->pgm_voltage_range MUST be "11.5-12.5V" ,no space between them.
* To program/read/verify/erase your MCU in Usual Voltage(4.5-5.5V)
* Your mcu->pgm_voltage_range MUST be "4.5-5.5V" ,no space between them.
* Author : Asif mahmud Shimon
*/
#ifndef OP_PIC18F_H
#define OP_PIC18F_H

#include <programmer.h>
#include <common.h>
#include <mcu_def.h>

#include <stdio.h>
#include <stdarg.h>

#define pic18f_name "pic18f"
#define pic18f_about "Microchip PIC18F programmer API\nBy Asif Mahmud Shimon\nVersion : 0.0.1"
#define pic18f_supported_family	mcu_families[MICROCHIP_18F].name
#define MAX_ERR 	200

#define pic18f_HV_PROG_EN_SIZE 48
#define pic18f_LV_PROG_EN_SIZE 61
#define pic18f_READ_CONFIG_SIZE 29
#define pic18f_EXIT_SIZE        7
#define HV_VREG                 12
#define pic18f_HV_PROG_EN_WRT_SIZE 38
#define pic18f_LV_PROG_EN_WRT_SIZE 51
#define pic18f_PRE_WRT_SIZE         16

extern UCHAR pic18f_HV_PROG_EN[pic18f_HV_PROG_EN_SIZE];
extern UCHAR pic18f_LV_PROG_EN[pic18f_LV_PROG_EN_SIZE];
extern UCHAR pic18f_READ_CONFIG[pic18f_READ_CONFIG_SIZE];
extern UCHAR pic18f_EXIT[pic18f_EXIT_SIZE];
extern UCHAR pic18f_HV_PROG_EN_WRT[pic18f_HV_PROG_EN_WRT_SIZE];
extern UCHAR pic18f_LV_PROG_EN_WRT[pic18f_LV_PROG_EN_WRT_SIZE];
extern UCHAR pic18f_PRE_WRT[pic18f_PRE_WRT_SIZE];

int pic18f_write(opprog_t*op, mcu_info_t *mcu,  op_mem_options_t *options);
int pic18f_read(opprog_t*, mcu_info_t *, op_mem_options_t *);
int pic18f_verify( mcu_info_t *, OP_MEM_TYPE, op_mcu_mem_t *);
int pic18f_erase(opprog_t*, mcu_info_t *, op_mem_options_t *);
void pic18f_register_update_cb(OP_FUNC_WRAP(void, update_cb)(const char *,const char *,int));
static void (*pic18f_update_cb)(const char *, const char *, int);

/**
* A wraper around the at_update callback
*/
void pic18f_log(const char *, const char *, ...);

/**
* Another wraper to show progress
*/
void pic18f_progress(const char *, int);

/**
 *STart HV regulator
 */
float pic18f_start_HV_reg(opprog_t * op);


#endif //OP_PIC18F_H
