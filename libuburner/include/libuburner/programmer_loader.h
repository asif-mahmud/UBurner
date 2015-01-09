/**
* programmer_loader.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_PROGRAMMER_LOADER_H
#define OP_PROGRAMMER_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <programmer.h>
#include <file_util.h>
#include <common.h>

/**
* A linked list to be filled-up by calling load programmers functions
*/
typedef struct programmer_list_t
{
    programmer_t * pgm;
	struct programmer_list_t * next;
}programmer_list_t;

/**
* Find a specific programmer in the given list and return the pointer
* @param list : the linked list to be searched for programmers
* @param name : name string of the target programmer_t
* @return : if found the programmer_t pointer else NULL
*/
programmer_t * find_programmer_by_name(programmer_list_t * , const char * );

/**
* Find a specific programmer in the given list and return the pointer
* @param list : the linked list to be searched for programmers
* @param family : family string of the target programmer_t
* @return : if found the programmer_t pointer else NULL
*/
programmer_t * find_programmer_by_family(programmer_list_t * , const char * );

/**
 * @brief load_programmers
 * @return loaded programmers
 */
programmer_list_t * load_programmers(int * );

#ifdef __cplusplus
}
#endif

#endif//OP_PROGRAMMER_LOADER_H
