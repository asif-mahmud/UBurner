#include <mcu_loader.h>
#include <file_util.h>
#include <util.h>
#include <sxmlc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>


mcu_family_name_t mcu_families[AVAILABLE_MCU_FAMILY_NUM] = 
{
    {"Microchip 10F"},
    {"Microchip 12F"},
    {"Microchip 16F"},
    {"Microchip 18F"},
    {"Microchip 24F"},
    {"Microchip 24H"},
    {"Microchip 24E"},
    {"Microchip 30F"},
    {"Microchip 33F"},
    {"Microchip 33E"},
	{"Atmel"},
	{"EEPROM"}
};


#define MALLOC_FOR_PIN_MAP( X ) X=(pin_map_t *)malloc(sizeof(pin_map_t));\
            X->name = (char *)calloc(8,1);\
            X->number = 0;\
			X->next = NULL
#define MALLOC_FOR_MCU_LIST( X ) X = (mcu_list_t *)malloc(sizeof(mcu_list_t));\
            X->mcu = (mcu_info_t *)malloc(sizeof(mcu_info_t));\
            MALLOC_FOR_PIN_MAP(X->mcu->pin_map);\
            X->next = NULL

OP_FUNC_WRAP(void , mcu_list_update)(const char *, const char *, int );

/**
* parse pgm_pin_map string
* pin map should be like this -
*	1:RESET;7:VCC;8:GND
*/
void parse_pin_map(char * mapping, pin_map_t * pins)
{
    char * tmp_mapping = (char *)calloc(strlen(mapping),1);
    memcpy(tmp_mapping,mapping,strlen(mapping));
    char * token = strtok(tmp_mapping,";");
    while(token != NULL)
    {
        pin_map_t * tmp_map;
        MALLOC_FOR_PIN_MAP(tmp_map);
        char number[4],name[8];
        for(int i = 0;i<strlen(token);i++)
        {
            if(token[i] == ':')
            {
                memcpy(number,token,i);
                memcpy(name,token+i+1,strlen(token)-i);
                break;
            }
        }
        tmp_map->number = atoi(number);
        memcpy(tmp_map->name,name,strlen(name));


        if(pins->number == 0)
        {
            pins->number = tmp_map->number;
            memcpy(pins->name,tmp_map->name,strlen(tmp_map->name));
        }else
        {
            pin_map_t * tmp = pins;
            while(tmp->next)
            {
                tmp = tmp->next;
            }
            tmp->next = tmp_map;
        }

        token = strtok(NULL,";");
    }
}

/**
* This function doesn't check for errors or doesn't do any malloc stuff
* So all these stuffs MUST be done before calling it
*/
void parse_mcu(XMLNode * mcu, mcu_list_t * list)
{
	XMLNode * node;
	char tmp[128];
	int ret;
	int i;
#define _STR_EQS(_x,_y) (strcasecmp(_x,_y)==0)
	for(i=0;i<mcu->n_children;i++)
	{
		node = mcu->children[i];
		if(_STR_EQS(node->tag,"id"))
			strtrim(node->text,list->mcu->id);
		else if(_STR_EQS(node->tag,"name"))
			strtrim(node->text,list->mcu->name);
		else if(_STR_EQS(node->tag,"family"))
			strtrim(node->text,list->mcu->family);
		else if(_STR_EQS(node->tag,"operating_voltage_range"))
			strtrim(node->text,list->mcu->operating_voltage_range);
		else if(_STR_EQS(node->tag,"pgm_voltage_range"))
			strtrim(node->text,list->mcu->pgm_voltage_range);
		else if(_STR_EQS(node->tag,"pgm_pin_map"))
		{
			strtrim(node->text,list->mcu->pgm_pin_map);
			if(strlen(list->mcu->pgm_pin_map)>0)
			{
                //MALLOC_FOR_PIN_MAP(list->mcu->pin_map);
				parse_pin_map(list->mcu->pgm_pin_map,list->mcu->pin_map);
			}
		}
		else if(_STR_EQS(node->tag,"speed_grade"))
			strtrim(node->text,list->mcu->speed_grade);
		else if(_STR_EQS(node->tag,"flash_memory_size"))
		{
			ret = strtrim(node->text,tmp);
			if(ret>0)
				list->mcu->flash_memory_size = atoi(tmp);
		}
		else if(_STR_EQS(node->tag,"data_memory_size"))
		{
			ret = strtrim(node->text,tmp);
			if(ret>0)
				list->mcu->data_memory_size = atoi(tmp);
		}
		else if(_STR_EQS(node->tag,"ram_size"))
		{
			ret = strtrim(node->text,tmp);
			if(ret>0)
				list->mcu->ram_size = atoi(tmp);
		}
		else if(_STR_EQS(node->tag,"package_type"))
		{
			int tmp2;
			ret = strtrim(node->text,tmp);
			if(ret>0)
			{
				tmp2 = atoi(tmp);
				switch(tmp2)
				{
					case PDIP :
						list->mcu->package.package = PDIP;
						break;
					default :
						list->mcu->package.package = NONE;
						break;
				}
			}
		}
		else if(_STR_EQS(node->tag,"pin_count"))
		{
			ret = strtrim(node->text,tmp);
			if(ret > 0)
				list->mcu->package.pin_count = atoi(tmp);
        }else if(_STR_EQS(node->tag,"pgm_page_size"))
        {
            list->mcu->pgm_page_size = (int)strtol(node->text,NULL,10);//10 base
        }else if(_STR_EQS(node->tag,"erase_word_1"))
        {
            list->mcu->erase_word_1 = (int)strtol(node->text,NULL,16);//hex value i.e ABe308
        }else if(_STR_EQS(node->tag,"erase_word_2"))
        {
            list->mcu->erase_word_2 = (int)strtol(node->text,NULL,16);//hex value i.e ABe308
        }
	}	
}

mcu_list_t * parse_mcu_config(file_info_t * file, int * total)
{
	mcu_list_t * ret = NULL;
	int ret_check;	
	char str[512];
	int prev_total = (*total);
	if(mcu_list_update)
	{
		sprintf(str,"Parsing file %s ...",file->name_with_ext);
		mcu_list_update(__FUNCTION__,str,0);
	}

	XMLDoc doc;
	XMLDoc_init(&doc);
	ret_check = XMLDoc_parse_file(file->name,&doc);
	if(ret_check)//Document is parsed
	{
		if( 	(doc.n_nodes>=1) && 
			(doc.i_root>=0) && 
			(strcasecmp(doc.nodes[doc.i_root]->tag,"mcu_list")==0))//check if it is mcu_list or mcu tag
		{
			//TODO : there MUST be one and only one mcu_list tag the other one won't be checked this way. **FIX ME**
			XMLNode *mcu;
			if(doc.nodes[doc.i_root]->n_children>0)//n_children == Number of mcu tags
			{
				int i;
				for(i=0;i<doc.nodes[doc.i_root]->n_children;i++)
				{
					mcu_list_t * tmp;
					MALLOC_FOR_MCU_LIST(tmp);
					mcu = doc.nodes[doc.i_root]->children[i];
					parse_mcu(mcu,tmp);
					(*total)++;

					if(!ret)
						ret = tmp;
					else
					{
						mcu_list_t * tmp2 = ret;
						while(tmp2->next){tmp2=tmp2->next;}
						tmp2->next = tmp;
					}
				}
				if(mcu_list_update)
				{
					sprintf(str,"Found %d MCUs/Chips in %s.",(*total)-(prev_total),file->name_with_ext);
					mcu_list_update(__FUNCTION__,str,*total);
				}
			}
		}
		else
		{
			if(mcu_list_update)
			{
				sprintf(str,"No root element found in %s !",file->name_with_ext);
				mcu_list_update(__FUNCTION__,str,0);
			}
		}		
	}
	else//Wrong document
	{
		if(mcu_list_update)
		{
			sprintf(str,"Wrong file %s !",file->name_with_ext);
			mcu_list_update(__FUNCTION__,str,0);
		}
	}

	XMLDoc_free(&doc);
	
	return ret;
}

mcu_list_t * load_mcu_list( const char * path , int * total )
{
	mcu_list_t * ret = NULL;
    char str[512];
	if(mcu_list_update)
	{
		*total = 0;
		sprintf(str,"Loading MCU list from %s ...", path);
		mcu_list_update(__FUNCTION__,str,*total);
	}

	file_list_t * files = get_file_list(path);
	if(files)
	{
		if(mcu_list_update)
		{
			int file_n =0;
			file_list_t * tmp = files;
			for(;tmp;tmp=tmp->next)
			{file_n++;}
            sprintf(str,"Found %d files in %s ...",file_n,path);
			mcu_list_update(__FUNCTION__,str,*total);
		}
		
		for(;files;files=files->next)
		{
			mcu_list_t * tmp = parse_mcu_config(files->file,total);
			if(mcu_list_update)
			{
				sprintf(str,"%s is parsed. Total MCUs/Chips : %d",files->file->name_with_ext,(*total));
				mcu_list_update(__FUNCTION__,str,*total);
			}

			if(!ret)
				ret = tmp;
			else
			{
				mcu_list_t * tmp2 = ret;
				while(tmp2->next){tmp2=tmp2->next;}
				tmp2->next = tmp;
			}
		}
	}
	return ret;
}


mcu_info_t * find_mcu(mcu_list_t * list, const char * _name)
{
	mcu_info_t * ret = NULL;
	mcu_list_t * tmp = list;
	char str[512];	
	if(mcu_list_update)
	{
		sprintf(str,"Looking for %s in the loaded MCU list...",_name);
		mcu_list_update(__FUNCTION__,str,0);
	}
	for(;tmp;tmp=tmp->next)
	{
		if(strcasecmp(tmp->mcu->name,_name)==0)
		{
			if(mcu_list_update)
			{
				sprintf(str,"Found MCU/Chip named as %s",_name);
				mcu_list_update(__FUNCTION__,str,0);
			}
			ret = tmp->mcu;
			break;
		}
	}
	if((!ret) &&(mcu_list_update))
	{
		sprintf(str,"Could not find MCU/Chip named as %s !",_name);
		mcu_list_update(__FUNCTION__,str,0);
	}
	return ret;
}

void register_mcu_list_update_cb(OP_FUNC_WRAP(void , update_cb)(const char * , const char *, int) )
{
	if(update_cb ==NULL) printf("It's NULL !!!\n");
	//mcu_list_update = update_cb;
}


void free_mcu_list(mcu_list_t * list)
{
	for(;list;list=list->next)
	{
		free(list->mcu);
	}
	if(list)
		free(list);
}
