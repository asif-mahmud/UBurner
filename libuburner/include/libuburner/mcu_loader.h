/**
* mcu_loader.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_MCU_LOADER_H
#define OP_MCU_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mcu_def.h>
#include <file_def.h>
#include <common.h>

/**
* Parse a configuration file by file_info_t structure
* @param file : file_info_t containing the file info to be parsed
* @param total : total MCUs/Chips found
* @return : an mcu_list_t pointer else NULL
*/
mcu_list_t * parse_mcu_config(file_info_t *, int *);

/**
* To find/locate available MCU/Memory List Configuration Script
* @param dir : const char directory name to be searched in
* @param total : total mcu structures loaded, 0 for NONE else some positive integer
* @return : An mcu_list_t structure if any configuration is found else NULL
*/
mcu_list_t * load_mcu_list( const char * , int * );

/**
* Free-up the memory after using mcu_list_t loaded by load_mcu_list
* @param list : mcu_list_t pointer to be freed
*/
void free_mcu_list(mcu_list_t * );

/**
* Find an MCU by it's name. It's case insensitive
* @param list : list to be searched in
* @param name : name of the target mcu
* @return : the mcu_info_t structure containg the mcu's info else NULL
*/
mcu_info_t * find_mcu(mcu_list_t * , const char *);

/**
* Register an update callback for mcu_list loader
* Arguments of the update call back are - 
*	sender : function name __FUNCTION__
*	argv : update string
*	d_val : additional value
* So USER defined callvack's protoype is like this 
*	void mcu_update_cb(const char *, const char *, int );
* And pass to the register like this -
* 	register_mcu_list_update_cb( mcu_update_cb );
*/
void register_mcu_list_update_cb( OP_FUNC_WRAP(void , update_cb)(const char * , const char *, int) );

/**
* Update callbacks pointer to be used internally
*/
extern OP_FUNC_WRAP(void, mcu_list_update)(const char * , const char *, int);


#ifdef __cplusplus
}
#endif

#endif//OP_MCU_LOADER_H
