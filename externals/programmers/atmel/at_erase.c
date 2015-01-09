#include <atmel.h>
#include <string.h>

UCHAR at_LV_ERASE[at_LV_ERASE_SIZE] = {
    SPI_WRITE, 0x04, 0xAC, 0x80, 0x00, 0x00,
    SET_PARAMETER, SET_T3,0x13,0x88, FLUSH
};

int atmel_erase(opprog_t* op, mcu_info_t *mcu, op_mem_options_t * options)
{
    int ret = 0,cnt,cnt2;
    op_buffer_t buffer_out, buffer_in;
    buffer_out.len = OP_PGM_BUFFER_SIZE;
    buffer_out.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
    buffer_in.len = OP_PGM_BUFFER_SIZE;
    buffer_in.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));

    memcpy(buffer_out.data+1,at_LV_CLK,at_LV_CLK_SIZE);
    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    at_log(__FUNCTION__,"Synchronising ...");
    for(cnt=0;cnt<32;cnt++)
    {
        memset(buffer_out.data,0x00,buffer_out.len);
        memcpy(buffer_out.data+1,at_LV_PGM_EN,at_LV_PGM_EN_SIZE);

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

        for(cnt2=1;(cnt2<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt2]!=SPI_READ);cnt2++);
        if(buffer_in.data[cnt2+2]==0x53) cnt = 32;
    }
    if(cnt <33)
    {
        memset(buffer_out.data,0x00,buffer_out.len);
        memcpy(buffer_out.data+1,at_LV_SPI_EXIT,at_LV_SPI_EXIT_SIZE);

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

        at_log(__FUNCTION__,"synchronization error !!! %d",cnt);
        return -1;
    }
    at_log(__FUNCTION__,"synchronizaton successful ! %d",cnt);

    //erase first
    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,at_LV_ERASE,at_LV_ERASE_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,at_LV_EXIT,at_LV_EXIT_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    at_log(__FUNCTION__,"Chip erased");

    return 0;
}
