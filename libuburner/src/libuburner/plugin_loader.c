/**
* plugin_loader.c
* Author : Asif Mahmud SHimon
*/
#include <plugin_loader.h>
#include <dlfcn.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MALLOC_FOR_PLUGIN_LIST(X) X=(plugin_list_t *)malloc(sizeof(plugin_list_t));\
					X->plugin = (plugin_t *)malloc(sizeof(plugin_t));\
					X->next = NULL

#define _STR_EQS(_x,_y) strcmp(_x,_y)==0

OP_FUNC_WRAP(void, plugin_load_update)(const char *, const char *, int);

void close_plugins(plugin_list_t * list)
{
	if(list)
	{
		plugin_list_t * tmp = list;
		for(;tmp;tmp=tmp->next)
		{
			dlclose(tmp->plugin_hnd);
			free(tmp->plugin);
		}
		free(list);
	}
}

plugin_list_t * load_plugin_from_file(file_info_t * file)
{
	int  check = 0;
	char str[512],str_check[128];
	plugin_list_t * ret = NULL;
	void(*init_plugin)(plugin_t *);

	sprintf(str_check,"%s%s",file->name_without_ext,PLUGIN_SUFFIX);
	if(_STR_EQS(str_check,file->name_with_ext)) check = 1;
	else
	{
		if(plugin_load_update)
		{
			sprintf(str,"Plugin suffix mismatched. Given : [%s], Req: [%s]",file->name_with_ext,str_check);
			plugin_load_update(__FUNCTION__,str,0);
		}
	}

	if(check)
	{
		if(plugin_load_update)
		{
			sprintf(str,"Loading plugin from %s",file->name);
			plugin_load_update(__FUNCTION__,str,0);
		}

		MALLOC_FOR_PLUGIN_LIST(ret);
		ret->plugin_hnd = dlopen(file->name, RTLD_LAZY);
		if(ret->plugin_hnd)
		{
			if(plugin_load_update)
			{
				sprintf(str,"Handle creation SUCCESSFUL");
				plugin_load_update(__FUNCTION__,str,0);
			}
			
			sprintf(str_check,"%s%s",file->name_without_ext,"_init");
			init_plugin = dlsym(ret->plugin_hnd,str_check);
			if(init_plugin)
			{
				init_plugin(ret->plugin);
				if(plugin_load_update)
				{
					sprintf(str,"Plugin %s Loaded",file->name_without_ext);
					plugin_load_update(__FUNCTION__,str,0);
				}
			}
			else
			{
				if(plugin_load_update)
				{
					sprintf(str,"ERROR : %s",dlerror());
					plugin_load_update(__FUNCTION__,str,0);
					sprintf(str,"ERROR loading %s",str_check);
					plugin_load_update(__FUNCTION__,str,0);
				}
				ret = NULL;
			}
		}
		else
		{
			if(plugin_load_update)
			{
				sprintf(str,"ERROR Loading plugin from %s",file->name);
				plugin_load_update(__FUNCTION__,str,0);
				sprintf(str,"ERROR : %s",dlerror());
				plugin_load_update(__FUNCTION__,str,0);
			}
			ret = NULL;
		}
	}

	return ret;
}

void register_plugin_load_update_cb(OP_FUNC_WRAP(void ,update_cb)(const char *, const char *, int))
{
	plugin_load_update = update_cb ;
}

plugin_t * find_plugin(plugin_list_t * list, const char * name)
{
	plugin_t * ret = NULL;
	plugin_list_t * tmp = list;
	char str[512];
	
	for(;tmp;tmp=tmp->next)
	{
		if(_STR_EQS(tmp->plugin->name,name))
		{
			ret = tmp->plugin;
			if(plugin_load_update)
			{
				sprintf(str,"Found plugin %s",name);
				plugin_load_update(__FUNCTION__,str,1);
			}
			break;
		}
	}

	return ret;
}

plugin_list_t * load_plugins(const char * dir, int * total)
{
	plugin_list_t * ret = NULL;
	file_list_t * files = get_file_list(dir);
	char str[512];
	
	if(plugin_load_update)
	{
		sprintf(str,"Looking for plugins in %s",dir);
		plugin_load_update(__FUNCTION__,str,0);
	}

	*total = 0;

	for(;files;files=files->next)
	{
		plugin_list_t *tmp = load_plugin_from_file(files->file);
		if(tmp)
		{
			++(*total);	
			if(!ret)
				ret = tmp;
			else
			{
				plugin_list_t * tmp2 = ret;
				while(tmp2->next){tmp2=tmp2->next;}
				tmp2->next = tmp;
			}
		}
	}

	if(plugin_load_update)
	{
		sprintf(str,"Total %d plugins found in %s",*total,dir);
		plugin_load_update(__FUNCTION__,str,*total);
	}


	return ret;
}

