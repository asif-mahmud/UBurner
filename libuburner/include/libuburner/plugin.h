/**
* plugin.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_PLUGIN_H
#define OP_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <opprog.h>
#include <mcu_def.h>

/**
* Say you want to provide a my_lib named plugin. So you include this header
* in your source and you dont need to link against libopprog.
* To be a plugin your plugin MUST have the followings -
* 	- a function named : void my_lib_init( plugin_t *);
*	- your plugin MUST be named as my_lib.so/my_lib.dll/my_lib.dylib 
*	  according to your OS specifications.
*	- the name variable of the structure MUST be my_lib
*	- there MUST be a function named register_update_cb( OP_FUNC_WRAP(void ,update_cb)(const char *, const char *, int) )
*	  to register a update callback for your plugin, in your provided plugin_t structure by my_lib_init function. This will
*	  not be called the user directly, rather user will call plugin_t->register_update_cb. So in your init function 
*	  you MUST point this pointer to  your register function.
*	- there MUST be a function named my_lib_run(opprog_t * op,mcu_info_t * mcu,char * argv, int d_val) to run your plugin. 
*	  Like register_update_cb you MUST point a run function to your plugin_t->run.
* The name variable is restricted to be exactly the same as my_lib. But the Screen name can be any name you want to 
* show on the screen or on the GUI toolbar/menubar. When caller program calls your plugin's run function it may/may not
* provide any of the arguments at all. If it provides opprog_t that MUST be the current opprog_t object. Same applies to
* the mcu_info_t object. The argument argv is yet to developed.
*/
typedef struct 
{
	char		name[32];//name of the plugin
	char		type[32];//type of the plugin
	char 		screen_name[32];//screen name for GUI
	
	/**
	* run function for your plugin. Programm will call this when it is necessary to run 
	* your plugin. It can contain anything even another GUI program if you want
	* @param op : opprog_t object
	* @param mcu : an mcu_config_t object pointer
	* @param argv : arguments passed by the caller program
	* @param d_val : additional value
	*/
	OP_FUNC_WRAP( void, run )( opprog_t *, mcu_info_t *,char *, int);
}plugin_t;

#ifdef __cplusplus
}
#endif

#endif//OP_PLUGIN_H
