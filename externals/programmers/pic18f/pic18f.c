/**
* atmel.c
* Author : Asif Mahmud Shimon
*/
#include <pic18f.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <mcu_def.h>
#include <stdint.h>

void pic18f_register_update_cb( OP_FUNC_WRAP(void, update_cb)(const char *,const char *,int))
{
    pic18f_update_cb = update_cb;
}

void pic18f_log(const char * name, const char * format, ...)
{
    char str[512];
    va_list args;
    va_start(args,format);
    vsprintf(str,format,args);
    va_end(args);
    if(pic18f_update_cb)pic18f_update_cb(name,str,0);
    Sleep(50);
}

void pic18f_progress(const char * name, int val)
{
    if(pic18f_update_cb)pic18f_update_cb(name,"progress",val);
}


float pic18f_start_HV_reg(opprog_t *op)
{
    pic18f_log(__FUNCTION__,"Starting HV regulator");

    uint8_t VREG = HV_VREG*10;
    int j = 1;
    int ret;
    op_buffer_t buffer_out, buffer_in;
    buffer_out.len = OP_PGM_BUFFER_SIZE;
    buffer_out.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
    buffer_in.len = OP_PGM_BUFFER_SIZE;
    buffer_in.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));

    buffer_out.data[j++] = VREG_EN;
    buffer_out.data[j++] = SET_VPP;
    buffer_out.data[j++] = VREG;
    buffer_out.data[j++] = SET_PARAMETER;
    buffer_out.data[j++] = SET_T3;
    buffer_out.data[j++] = 0x07;
    buffer_out.data[j++] = 0xD0;
    buffer_out.data[j++] = WAIT_T3;
    buffer_out.data[j++] = READ_ADC;
    buffer_out.data[j++] = FLUSH;

    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    for(j=1;j<(OP_PGM_BUFFER_SIZE-2)&&buffer_in.data[j]!=READ_ADC;j++){}
    int volt = (buffer_in.data[j+1]<<8)+buffer_in.data[j+2];
    pic18f_log(__FUNCTION__,"V_ADC : %d",volt);
    if(volt == 0)
    {
        pic18f_log(__FUNCTION__,"HV Regulator Error, Use External Power Source");
        return -1;
    }
    for(int _try=0;_try<5;_try++)
    {
        memset(buffer_out.data,0x00,buffer_out.len);
        j=1;
        buffer_out.data[j++] = WAIT_T3;
        buffer_out.data[j++] = READ_ADC;
        buffer_out.data[j++] = FLUSH;

        if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

        for(j=1;j<(OP_PGM_BUFFER_SIZE-2)&&buffer_in.data[j]!=READ_ADC;j++){}
        volt = (buffer_in.data[j+1]<<8)+buffer_in.data[j+2];

        pic18f_log(__FUNCTION__,"V_REG : %f V",(float)volt/V_GAIN);
        if((volt/V_GAIN)>9)
            break;
    }
    if((volt/V_GAIN)<9)
    {
        pic18f_log(__FUNCTION__,"HV Regulator Error.");
        return -1;
    }else
        pic18f_log(__FUNCTION__,"HV regulator up and running at %f V",(volt/V_GAIN));

    return (volt/V_GAIN);
}
