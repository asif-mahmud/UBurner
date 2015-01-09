/**
* programmer.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_PROGRAMMER_H
#define OP_PROGRAMMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <opprog.h>
#include <mcu_def.h>
#include <instructions.h>

/**
* OpProg Hardware dependent Buffer size. Should be used in Porgrammer interfaces to allocate 
* buffer for USB communications.
*/
#ifdef _WIN32
	#define OP_PGM_BUFFER_SIZE 65
#else
	#define OP_PGM_BUFFER_SIZE 64
#endif

/**
* Common Programmer functionalities read, write or verify memories
* Programmers are linked statically . So to write a programmer
* you need to follow this -
*	Say you want to write a programmer for atmega series. So do this -
*	- provide 6 functions -->
*		- programmer_t * atmega_init() : to initialize the programmer
*		- read, write, verify, erase functionalities as described below
*		- a register_update_callback function as described below
* init function MUST asign read,write,verify,erase and register_update_cb functions so that
* they can be called through the programmer_t structure.
* To build a programmer include this header and link against libopprog
*/
typedef struct programmer_t
{
	char name[128];
	char about[256];
	char supported_family[128];
	
	/**
	 * @return : 0 if SUCCESS else -1
	 */
	OP_FUNC_WRAP( int, read )( opprog_t*, mcu_info_t *, op_mem_options_t * );
	
	/**
	 * @return : 0 if SUCCESS else -1
	 */
	OP_FUNC_WRAP( int, write )( opprog_t*, mcu_info_t *,  op_mem_options_t * );
	
	/**
	 * @return : 0 if SUCCESS else -1
	 */
	OP_FUNC_WRAP( int, erase )( opprog_t*, mcu_info_t *,  op_mem_options_t * );
	
	/**
	 * @return : 0 is SUCCEED else -1
	 */
	OP_FUNC_WRAP( int, verify )( mcu_info_t *, OP_MEM_TYPE, op_mcu_mem_t * );

	OP_FUNC_WRAP(void, register_update_cb)( OP_FUNC_WRAP(void, update_cb)(const char *,const char *,int));
}programmer_t;


#ifdef __cplusplus
}
#endif

#endif//OP_PROGRAMMER_H
