/**
* atmel.c
* Author : Asif Mahmud Shimon
*/
#include <atmel.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <mcu_def.h>

void atmel_register_update_cb( OP_FUNC_WRAP(void, update_cb)(const char *,const char *,int))
{
    at_update_cb = update_cb;
}

void at_log(const char * name, const char * format, ...)
{
    char str[512];
    va_list args;
    va_start(args,format);
    vsprintf(str,format,args);
    va_end(args);
    if(at_update_cb)at_update_cb(name,str,0);
    Sleep(50);
}

void at_progress(const char * name, int val)
{
    if(at_update_cb)at_update_cb(name,"progress",val);
}


int at_get_page_size(mcu_info_t * mcu)
{
    return mcu->pgm_page_size;
}
