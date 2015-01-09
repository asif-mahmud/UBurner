/**
* opprog.c
* Author : Asif Mahmud Shimon
*/
#include <opprog.h>
#include <common.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int op_find_device(USHORT vid, USHORT pid)
{
    hid_dev_info * dev;
	int ret = 0;

	//it's a function who doesn't need opprog_t structure
	//so don't look for that
    dev = hid_enumerate(vid,pid);
	if(dev)//device list found
    {
        ret = 1;
		hid_free_enumeration(dev);
	}

	return ret;
}

void op_get_device_info(opprog_t * op, USHORT vid, USHORT pid )
{
    if(op->dev_hnd)
    {
        op->dev_info->vid = vid;
        op->dev_info->pid = pid;
        hid_get_manufacturer_string(op->dev_hnd,op->dev_info->manufacturer_string,MAX_NAME_LEN);
        hid_get_product_string(op->dev_hnd,op->dev_info->product_string,MAX_NAME_LEN);
        hid_get_indexed_string(op->dev_hnd,1,op->dev_info->indexed_string,MAX_NAME_LEN);
        hid_get_serial_number_string(op->dev_hnd,op->dev_info->serial_number_string,MAX_NAME_LEN);
    }
}

void op_open_device(opprog_t * op, USHORT vid, USHORT pid)
{
    hid_device * dev = hid_open(vid,pid,NULL);
    if(dev)//found, now fill in the info structure
    {
        op->dev_hnd = dev;
        op->get_device_info(op,vid,pid);
    }
}


int op_send(opprog_t * op, op_buffer_t * buffer)
{
	int ret = -1;

	if(op->dev_hnd)
	{
		ret = hid_write(op->dev_hnd,buffer->data,buffer->len);
	}

	return ret;
}

int op_recv(opprog_t * op, op_buffer_t * buffer)
{
	int ret =-1;
	if(op->dev_hnd)
	{
		ret = hid_read(op->dev_hnd,buffer->data, buffer->len);
	}
	return ret;
}

int op_fill_up_memory(opprog_t * op, OP_MEM_TYPE mem_type,UCHAR * data, size_t size)
{
	UCHAR * tmp;
	int ret =-1;
#define MALLOC(X,TMP,len,chk_len) TMP=(UCHAR *)realloc(X,len);\
					if(TMP) {\
						X=TMP;\
						chk_len = len;\
					}
#define FILL_UP(dest,src,len,chk_len,RET) if(len==chk_len) {\
						memcpy(dest,src,len);\
						RET = 0;\
					}
	switch(mem_type)
	{
		case FLASH_READ :
		{
			MALLOC(op->memory->flash_read->data,tmp,size,op->memory->flash_read->len)
			FILL_UP(op->memory->flash_read->data,data,size,op->memory->flash_read->len,ret)
			break;
		}
		case FLASH_TO_WRITE :
		{
			MALLOC(op->memory->flash_to_write->data,tmp,size,op->memory->flash_to_write->len)
			FILL_UP(op->memory->flash_to_write->data,data,size,op->memory->flash_to_write->len,ret)
			break;
		}
		case FLASH_LOADED :
		{
			MALLOC(op->memory->flash_loaded->data,tmp,size,op->memory->flash_loaded->len)
			FILL_UP(op->memory->flash_loaded->data,data,size,op->memory->flash_loaded->len,ret)
			break;
		}
		case EEPROM_READ :
		{
			MALLOC(op->memory->eeprom_read->data,tmp,size,op->memory->eeprom_read->len)
			FILL_UP(op->memory->eeprom_read->data,data,size,op->memory->eeprom_read->len,ret)
			break;
		}
		case EEPROM_TO_WRITE :
		{
			MALLOC(op->memory->eeprom_to_write->data,tmp,size,op->memory->eeprom_to_write->len)
			FILL_UP(op->memory->eeprom_to_write->data,data,size,op->memory->eeprom_to_write->len,ret)
			break;
		}
		case EEPROM_LOADED :
		{
			MALLOC(op->memory->eeprom_loaded->data,tmp,size,op->memory->eeprom_loaded->len)
			FILL_UP(op->memory->eeprom_loaded->data,data,size,op->memory->eeprom_loaded->len,ret)
			break;
		}
		case CONFIG_READ :
		{
			MALLOC(op->memory->config_read->data,tmp,size,op->memory->config_read->len)
			FILL_UP(op->memory->config_read->data,data,size,op->memory->config_read->len,ret)
			break;
		}
		case CONFIG_TO_WRITE :
		{
			MALLOC(op->memory->config_to_write->data,tmp,size,op->memory->config_to_write->len)
			FILL_UP(op->memory->config_to_write->data,data,size,op->memory->config_to_write->len,ret)
			break;
		}
		case CONFIG_LOADED :
		{
			MALLOC(op->memory->config_loaded->data,tmp,size,op->memory->config_loaded->len)
			FILL_UP(op->memory->config_loaded->data,data,size,op->memory->config_loaded->len,ret)
			break;
		}
		default : 
			break;
	}

	return ret;
}

void op_clear_memory( opprog_t * op, OP_MEM_TYPE type )
{
#define CLEAR(buffer) if(!buffer->len)return;\
			free(buffer->data);\
			buffer->data = NULL;\
			buffer->len=0;

	switch(type)
	{
		case FLASH_READ :
			CLEAR(op->memory->flash_read)
			break;
		case FLASH_TO_WRITE :
			CLEAR(op->memory->flash_to_write)
			break;
		case FLASH_LOADED :
			CLEAR(op->memory->flash_loaded)
			break;
		case EEPROM_READ :
			CLEAR(op->memory->eeprom_read)
			break;
		case EEPROM_TO_WRITE :
			CLEAR(op->memory->eeprom_to_write)
			break;
		case EEPROM_LOADED :
			CLEAR(op->memory->eeprom_loaded)
			break;
		case CONFIG_READ :
			CLEAR(op->memory->config_read)
			break;
		case CONFIG_TO_WRITE :
			CLEAR(op->memory->config_to_write)
			break;
		case CONFIG_LOADED :
			CLEAR(op->memory->config_loaded)
			break;
		default :
			break;
	}
}

void op_append_data(opprog_t * op, OP_MEM_TYPE type, UCHAR * data, size_t _len)
{
	if(!_len)return;
#define APPEND(buffer,src,__len,TMP) TMP = (UCHAR *)realloc(buffer->data,buffer->len+__len); \
					if(TMP) { \
						buffer->data = TMP; \
						memcpy(buffer->data+buffer->len,src,__len); \
						buffer->len += __len; \
					}
	UCHAR * tmp ;
	switch(type)
	{
		case FLASH_READ :
			APPEND(op->memory->flash_read,data,_len,tmp)
			break;
		case FLASH_TO_WRITE :
			APPEND(op->memory->flash_to_write,data,_len,tmp)
			break;
		case FLASH_LOADED :
			APPEND(op->memory->flash_loaded,data,_len,tmp)
			break;
		case EEPROM_READ :
			APPEND(op->memory->eeprom_read,data,_len,tmp)
			break;
		case EEPROM_TO_WRITE :
			APPEND(op->memory->eeprom_to_write,data,_len,tmp)
			break;
		case EEPROM_LOADED :
			APPEND(op->memory->eeprom_loaded,data,_len,tmp)
			break;
		case CONFIG_READ :
			APPEND(op->memory->config_read,data,_len,tmp)
			break;
		case CONFIG_TO_WRITE :
			APPEND(op->memory->config_to_write,data,_len,tmp)
			break;
		case CONFIG_LOADED :
			APPEND(op->memory->config_loaded,data,_len,tmp)
			break;
		default :
			break;
	}	
}

opprog_t * op_init()
{
	opprog_t * ret = NULL;

	if(hid_init()==0)
	{
		ret = (opprog_t *)malloc(sizeof(opprog_t));
		ret->memory = (op_mcu_mem_t *)malloc(sizeof(op_mcu_mem_t));
		ret->dev_hnd = NULL;
        ret->dev_info = (op_hid_device_info *)malloc(sizeof(op_hid_device_info));

#define SET_MEM(X) X=(op_buffer_t *)malloc(sizeof(op_buffer_t))
		//setup the memory
		SET_MEM(ret->memory->flash_read);
		ret->memory->flash_read->data = NULL;
		ret->memory->flash_read->len = 0;

		SET_MEM(ret->memory->flash_to_write);
		ret->memory->flash_to_write->data = NULL;
		ret->memory->flash_to_write->len = 0;

		SET_MEM(ret->memory->flash_loaded);
		ret->memory->flash_loaded->data = NULL;
		ret->memory->flash_loaded->len = 0;

		SET_MEM(ret->memory->eeprom_read);
		ret->memory->eeprom_read->data = NULL;
		ret->memory->eeprom_read->len = 0;

		SET_MEM(ret->memory->eeprom_to_write);
		ret->memory->eeprom_to_write->data = NULL;
		ret->memory->eeprom_to_write->len =0;

		SET_MEM(ret->memory->eeprom_loaded);
		ret->memory->eeprom_loaded->data = NULL;
		ret->memory->eeprom_loaded->len = 0;

		SET_MEM(ret->memory->config_read);
		ret->memory->config_read->data = NULL;
		ret->memory->config_read->len = 0;

		SET_MEM(ret->memory->config_to_write);
		ret->memory->config_to_write->data = NULL;
		ret->memory->config_to_write->len = 0;

		SET_MEM(ret->memory->config_loaded);
		ret->memory->config_loaded->data = NULL;
		ret->memory->config_loaded->len =0;

		//setup functions 
		ret->find_device = op_find_device ;
		ret->get_device_info = op_get_device_info ;
		ret->open_device = op_open_device ;
		ret->send = op_send ;	
		ret->recv = op_recv ;
		ret->fill_up_memory = op_fill_up_memory ;
		ret->clear_memory = op_clear_memory ;
		ret->append_data = op_append_data ;
	}

	return ret;
}

void op_exit(opprog_t * op)
{
	if(op->dev_hnd)//close the opened HID device
		hid_close(op->dev_hnd);
	if(op->dev_info)//free device info structure
		free(op->dev_info);

	hid_exit();

	//now free up memory parts
	free(op->memory->flash_read);
	free(op->memory->flash_to_write);
	free(op->memory->flash_loaded);

	free(op->memory->eeprom_read);
	free(op->memory->eeprom_to_write);
	free(op->memory->eeprom_loaded);

	free(op->memory->config_read);
	free(op->memory->config_to_write);
	free(op->memory->config_loaded);

	//finally free memory
	free(op->memory);
	
	free(op);
}
