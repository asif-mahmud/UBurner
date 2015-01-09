/**
* plugin_loader.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_PLUGIN_LOADER_H
#define OP_PLUGIN_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <plugin.h>
#include <file_util.h>
#include <common.h>

/**
* structure for plugins list generation
*/
typedef struct plugin_list_t
{
	plugin_t * plugin;
	void * plugin_hnd;
	struct plugin_list_t * next;
}plugin_list_t;

/**
* Function to load plugins from a given directory
* @param dir : directory to be searched in for plugins
* @param total : total plugins loaded
* @return : A Linked List of plugin_list_t structure
*/
plugin_list_t * load_plugins(const char *,int *);

/**
* Load plugin from a file. File MUST be .dll/.so/.dylib
* @param file : a file_info_t structure providing info about the file
* @return : the plugin_t else NULL if nothing in or error
* TODO : Can I check for the errors ? ***FIX ME*** 
*/
plugin_list_t * load_plugin_from_file(file_info_t *);

/**
* Free-up the plugin_list_t pointer loaded before
* @param list : The list to be freed
*/
void close_plugins(plugin_list_t *);

/**
* Find a plugin by it's name
* @param list : list to be searched in
* @param name : name of the desired plugin
* @return : if found the poiner of the plugin else NULL
*/
plugin_t * find_plugin( plugin_list_t * , const char *);


/**
* Register a callback for plugin loader updater
*/
void register_plugin_load_update_cb(OP_FUNC_WRAP(void ,update_cb)(const char *, const char *, int));


/**
* Update callback for internal use
*/
extern OP_FUNC_WRAP(void, plugin_load_update)(const char *, const char *, int);

#ifdef __cplusplus
}
#endif

#endif//OP_PLUGIN_LOADER_H
