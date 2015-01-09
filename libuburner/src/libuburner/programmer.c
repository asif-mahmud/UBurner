/**
* programmer_loader.c
* Author : Asif Mahmud Shimon
*/
#include <programmer_loader.h>
#include <stdio.h>
#include <string.h>

#define _STR_EQS(_x,_y) strcmp(_x,_y)==0

programmer_t * find_programmer_by_name(programmer_list_t * list, const char * name)
{
	programmer_t * ret = NULL;
	programmer_list_t * tmp = list;
	char str[512];
	
	for(;tmp;tmp=tmp->next)
	{
		if(_STR_EQS(tmp->pgm->name,name))	
		{
			ret = tmp->pgm;
			
			break;
		}
	}

	return ret;
}

programmer_t * find_programmer_by_family(programmer_list_t * list, const char * family)
{
	programmer_t * ret = NULL;
	programmer_list_t * tmp = list;
	char str[512];
	
	for(;tmp;tmp=tmp->next)
	{
		if(_STR_EQS(tmp->pgm->supported_family,family))	
		{
			ret = tmp->pgm;
			
			break;
		}
	}

	return ret;
}
