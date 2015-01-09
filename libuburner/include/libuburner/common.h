/**
* common.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_COMMON_H
#define OP_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#define Sleep(x) usleep((x*1000))
#endif

#define OP_FUNC_WRAP(ret, func)			ret (* func )
#define OP_FUNC_WRAP_PTR(ret_ptr, func) 	ret_ptr* (* func)

#define UCHAR 	unsigned char
#define USHORT	unsigned short
#define UINT 	unsigned int
#define ULONG	unsigned long

#define MAX_NAME_LEN    255

//This one borrowed from oggui
#define V_GAIN (12.0/34*1024/5)		//=72.2823529412

/**
* Abstract buffer structure for libopprog
* If 16 bit data is required use it in big-endian order
* means data[i]=LSB & data[i+1]=MSB
*/
typedef struct 
{
	UCHAR * data;
	size_t len;
}op_buffer_t;

/**
* Memory buffers of an Microcontroller
* Included --> Flash data, EEPROM, Configuration bytes
*/
typedef struct 
{
	op_buffer_t * flash_read;
	op_buffer_t * flash_to_write;
	op_buffer_t * flash_loaded;

	op_buffer_t * eeprom_read;
	op_buffer_t * eeprom_to_write;
	op_buffer_t * eeprom_loaded;

	op_buffer_t * config_read;
	op_buffer_t * config_to_write;
	op_buffer_t * config_loaded;
}op_mcu_mem_t;

/**
* Memory Types used in libopprog
*/
typedef enum
{
	FLASH_READ = 1,
	FLASH_TO_WRITE = 2,
	FLASH_LOADED = 3,
	EEPROM_READ = 4,
	EEPROM_TO_WRITE = 5,
	EEPROM_LOADED = 6,
	CONFIG_READ = 7,
	CONFIG_TO_WRITE = 8,
	CONFIG_LOADED = 9 ,
	FLASH_MEMORY = 10,
	EEPROM_MEMORY = 11,
    AVR_LOCK_READ = 12 ,
    AVR_LFUSE_READ = 13,
    AVR_HFUSE_READ = 14,
    AVR_EFUSE_READ = 15,
    AVR_LOCK_WRITE = 16 ,
    AVR_LFUSE_WRITE = 17,
    AVR_HFUSE_WRITE = 18,
    AVR_EFUSE_WRITE = 19,
    AVR_CAL = 20,
    AVR_SLOW = 21
}OP_MEM_TYPE;

/**
* Structure to be passed to the programmers
*/
typedef struct
{
    OP_MEM_TYPE * types;//types of memories
	int len;//length of types
}op_mem_options_t;

/**
* Structure to get hardware and firmware version
* of the programmer.
*/
typedef struct 
{
	int hardware_id;
	int firmware_version;
	char pgm_mcu[32];
}op_hardware_t;

/**
 * Configuration Bytes or memory positions in config_XXXX buffer
 */
#define AVR_ID_POS		0
#define AVR_LOCK_POS	3
#define AVR_LFUSE_POS 	4
#define AVR_HFUSE_POS	5
#define AVR_EFUSE_POS	6
#define AVR_CAL_POS		7
#define AVR_CONFIG_SIZE	11
#define PIC_ID_POS      0

#ifdef __cplusplus
}
#endif

#endif//OP_COMMON_H
