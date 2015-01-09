/**
* file_util.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_FILE_UTIL_H
#define OP_FILE_UTIL_H

#ifdef __cplusplus 
extern "C" {
#endif

#include <file_def.h>
#include <opprog.h>
#include <mcu_def.h>
#include <common.h>


/**
* Function to get a file_list_t pointer for a given directory
* @param dir : Director yto be searched in for files, . and .. are ignored
*/
file_list_t * get_file_list(const char * );

/**
* Function to save Flash/EEPROM data in a file.
* It will save flash_read and / or eeprom_read and / or config_read and / or config_loaded buffer. 
* So to use it fill up those buffers with the data to save. In details decribed here if it is marked as
* a MUST you can just fill the data buffer with nothing and set the len=0. libopprog provides a clear_memory
* function to do that. More details -
* 	For PIC10F, PIC12F, PIC16F --> 
*		flash_read - fill it with flash data and size. Use big-endian format.
*	**For PIC16F1xx -->
*		eeprom_data - fill it with eeprom data and size.
* 	For PIC18F --->
*		flash_read - fill it with flash data and size.
*		config_loaded - fill it with id memory. size should be 8 bytes. It's a MUST.
*		config_read - fill it with configuration bytes. size should be 14 bytes. It's a MUST.
*		eeprom_read - fill it with eeprom data and size.
* 	For PIC24F/H/E, PIC30F, PIC33F/E --->
*		flash_read - fill it with flash data. It's a MUST.
*		config_read - fill it with configuration data and size
*		eeprom_read - fill it with eeprom data and size.
*	For Atmel --->
*		flash_read - fill it with flash data and size.
*		eeprom_read - fill it with eeprom data and size.
*		**remark - 	User is not supposed to write flash and eeprom in the same file. so
*				do it seperately.
*	For EEPROM --->
*		eeprom_read - fill it with eeprom data and size.
*		
* @param op : an object of opprog_t containing the flash and eeprom to save
* @param file : file name to save in
* @param mcu : the target mcu
* @return : 0 on SUCCESS else -1
*/
int save_hex_data(opprog_t *, const char *, mcu_info_t *);

/**
* Function to read HEX data from a file. I successful the memory buffers
* of opprog_t i.e flash_loaded/eeprom_loaded/config_loaded will be filled up with the read data. 
* All memory sizes MUST be in Kbytes even in case of 14/12 bits data word.
* @param op : an object of opprog_t containing the flash and eeprom to save
* @param file : file name to save in
* @param mcu : the target mcu
* @return : 0 on SUCCESS else -1
*/
int read_hex_data(opprog_t *, const char *, mcu_info_t *);

#ifdef __cplusplus
}
#endif

#endif//OP_FILE_UTIL_H
