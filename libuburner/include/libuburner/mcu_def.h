/**
* mcu_def.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_MCU_DEF_H
#define OP_MCU_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/**
* Different sizes defined in Bytes
*/
#define	_512B	512
#define _1KB	1024
#define _2KB	(2*_1KB)
#define _4KB	(4*_1KB)
#define _8KB	(8*_1KB)
#define _16KB	(16*_1KB)
#define _32KB	(32*_1KB)
#define _64KB	(64*_1KB)
#define _128KB	(128*_1KB)

/**
* Currently Available programmer interfaces
*/
#define AVAILABLE_MCU_FAMILY_NUM	12

typedef enum {
	MICROCHIP_10F,
	MICROCHIP_12F,
	MICROCHIP_16F,
	MICROCHIP_18F,
	MICROCHIP_24F,
	MICROCHIP_24H,
	MICROCHIP_24E,
	MICROCHIP_30F,
	MICROCHIP_33F,
	MICROCHIP_33E,
	ATMEL,
	EEPROM
}mcu_family_t;


/**
* A structure for MCU/Memory Family name
*/
typedef struct {
	char name[32];
}mcu_family_name_t;


/**
* MCU/Memory chip's PCB Package enumeration
*/
typedef enum {
	NONE,
	PDIP
}mcu_package_type_t;

/**
* MCU/Memory Chip's PCB package structure
*/
typedef struct {
	mcu_package_type_t package;
	int pin_count;
}mcu_package_t;

/**
* Globally Available MCU/Memory Chip Family Names
*/
extern mcu_family_name_t mcu_families[AVAILABLE_MCU_FAMILY_NUM];

/**
* Pin mapping
*/
typedef struct pin_map_t
{
	int number;
    char * name;
	struct pin_map_t * next;
}pin_map_t;

/**
* MCU/Memory Definitions Structure
* Programmers will need one info structure to do it's operations
*/
typedef struct mcu_info_t{
	char 		id[16];//id to be used internally
	char 		name[32];//name of the mcu
	char 		family[32];//family name
	int 		flash_memory_size;//flash memory size
	int 		data_memory_size;//eeprom size
	int 		ram_size;//ram/sram size
    int         pgm_page_size;//programming page size
    int         erase_word_1;//erase instruction
    int         erase_word_2;//erase instruction
	char 		operating_voltage_range[16];
	char		pgm_voltage_range[16];
	char 		pgm_pin_map[128];//pin mapping for the programming pins
	pin_map_t 	* pin_map;//use this to see the pgm pin mapping
	char 		speed_grade[10];//mcu speed grade
	mcu_package_t 	package;
}mcu_info_t;

/**
* A Linked List for currently implemented or available MCU/Memory Chips
*/
typedef struct mcu_list_t
{
	mcu_info_t * mcu;
	struct mcu_list_t * next;
}mcu_list_t;


#ifdef __cplusplus
}
#endif

#endif//OP_MCU_DEF_H
