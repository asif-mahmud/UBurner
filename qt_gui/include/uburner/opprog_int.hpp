/**
* opprog_int.hpp
* Author : Asif Mahmud Shimon
*/
#ifndef OP_INT_HPP
#define OP_INT_HPP

#include <opprog.h>
#include <plugin_loader.h>
#include <programmer_loader.h>
#include <mcu_loader.h>
#include <file_util.h>
#include <util.h>
#include <stdint.h>

#include <map>
#include <vector>

using namespace std;

extern opprog_t 		* opprog;
extern mcu_list_t 		* mcus;
extern programmer_list_t 	* pgms;
extern plugin_list_t 		* plugins;
extern uint8_t          last_flash_action;
extern uint8_t          last_eeprom_action;
extern uint8_t          last_config_action;

#define LAST_FLASH_LOADED   0x01
#define LAST_FLASH_READ     0x02
#define LAST_EEPROM_LOADED  0x03
#define LAST_EEPROM_READ    0x04
#define LAST_CONFIG_LOADED  0x05
#define LAST_CONFIG_READ    0x06

extern map< int , vector<mcu_info_t *> > mapped_mcus;
extern op_mem_options_t memory_options;
typedef struct {
    UCHAR read_lock;
    UCHAR read_lfuse;
    UCHAR read_hfuse;
    UCHAR read_efuse;
    UCHAR write_lock;
    UCHAR write_lfuse;
    UCHAR write_hfuse;
    UCHAR write_efuse;
}_AVR_read_config_operations_t;

extern _AVR_read_config_operations_t avr_read_config_operations;

//callbacks should be here
void loader_update_cb(const char *, const char *, int);

//callback for programmer updates
void pgm_update_cb(const char *, const char *, int );

#endif//OP_INT_HPP
