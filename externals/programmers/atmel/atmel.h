/**
* atmel.h
* To program/read/verify/erase your MCU in High Voltage(11.5-12.5V)
* Your mcu->pgm_voltage_range MUST be "11.5-12.5V" ,no space between them.
* To program/read/verify/erase your MCU in Usual Voltage(4.5-5.5V)
* Your mcu->pgm_voltage_range MUST be "4.5-5.5V" ,no space between them.
* Author : Asif mahmud Shimon
*/
#ifndef OP_ATMEL_H
#define OP_ATMEL_H

#include <programmer.h>
#include <common.h>
#include <mcu_def.h>

#include <stdio.h>
#include <stdarg.h>

#define atmel_name "atmel"
#define atmel_about "Atmel programmer API\nBy Asif Mahmud Shimon\nVersion : 0.0.1"
#define atmel_supported_family	mcu_families[ATMEL].name
#define MAX_ERR 	200

#define  LOCK	1
#define  FUSE	2
#define  FUSE_H 4
#define  FUSE_X	8
#define  CAL	16
#define  SLOW	256
#define  RST    0x40

#define at_LV_CLK_SIZE          21
#define at_LV_PGM_EN_SIZE       22
#define at_LV_SPI_EXIT_SIZE     10
#define at_LV_SIG_READ_SIZE     21
#define at_LV_LOCK_READ_SIZE    7
#define at_LV_LFUSE_READ_SIZE   7
#define at_LV_HFUSE_READ_SIZE   7
#define at_LV_EFUSE_READ_SIZE   7
#define at_LV_CAL_READ_SIZE     29
#define at_LV_EXIT_SIZE         11
#define at_LV_ERASE_SIZE        11


extern UCHAR at_LV_CLK[at_LV_CLK_SIZE];
extern UCHAR at_LV_PGM_EN[at_LV_PGM_EN_SIZE];
extern UCHAR at_LV_SPI_EXIT[at_LV_SPI_EXIT_SIZE];
extern UCHAR at_LV_SIG_READ[at_LV_SIG_READ_SIZE];
extern UCHAR at_LV_LOCK_READ[at_LV_LOCK_READ_SIZE];
extern UCHAR at_LV_LFUSE_READ[at_LV_LFUSE_READ_SIZE];
extern UCHAR at_LV_HFUSE_READ[at_LV_HFUSE_READ_SIZE];
extern UCHAR at_LV_EFUSE_READ[at_LV_EFUSE_READ_SIZE];
extern UCHAR at_LV_CAL_READ[at_LV_CAL_READ_SIZE];
extern UCHAR at_LV_EXIT[at_LV_EXIT_SIZE];
extern UCHAR at_LV_ERASE[at_LV_ERASE_SIZE];


int atmel_write(opprog_t*op, mcu_info_t *mcu,  op_mem_options_t *options);
int atmel_read(opprog_t*, mcu_info_t *, op_mem_options_t *);
int atmel_verify( mcu_info_t *, OP_MEM_TYPE, op_mcu_mem_t *);
int atmel_erase(opprog_t*, mcu_info_t *, op_mem_options_t *);
void atmel_register_update_cb(OP_FUNC_WRAP(void, update_cb)(const char *,const char *,int));
static void (*at_update_cb)(const char *, const char *, int);

/**
* A wraper around the at_update callback
*/
void at_log(const char *, const char *, ...);

/**
* Another wraper to show progress
*/
void at_progress(const char *, int);

/**
* Function to set up read/write options
*/
void set_options(op_mem_options_t * options, UCHAR * _lock,UCHAR * _l_fuse,UCHAR * _h_fuse,UCHAR * _e_fuse,UCHAR * _cal,UCHAR * _slow);

/**
* Function to get write page size
*/
int at_get_page_size(mcu_info_t * _mcu);

#endif //OP_ATMEL_H
