#include <pic18f.h>
#include <string.h>
#include <stdint.h>


int pic18f_erase(opprog_t* op, mcu_info_t *mcu, op_mem_options_t * options)
{
    int ret;
    op_buffer_t buffer_out, buffer_in;
    buffer_out.len = OP_PGM_BUFFER_SIZE;
    buffer_out.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
    buffer_in.len = OP_PGM_BUFFER_SIZE;
    buffer_in.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));

#define CONTAINS(STR,X) (strchr(STR,X)!=NULL)

    uint8_t lv_prog = 0;
    uint8_t dummy = 0x00;
    if(CONTAINS(mcu->name,'J') || CONTAINS(mcu->name,'j')) lv_prog = 1;

    pic18f_log(__FUNCTION__,"VPP state needed[0->HV,1->LV]: %d",lv_prog);
    if(!lv_prog)
    {
        float reg = pic18f_start_HV_reg(op);
        if(reg<9.0) return -1;
    }

    memset(buffer_out.data,0x00,buffer_out.len);
    if(lv_prog) memcpy(buffer_out.data+1,pic18f_LV_PROG_EN_WRT,pic18f_LV_PROG_EN_WRT_SIZE);
    else memcpy(buffer_out.data+1,pic18f_HV_PROG_EN_WRT,pic18f_HV_PROG_EN_WRT_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    int z;
    for(z=0;z<OP_PGM_BUFFER_SIZE && buffer_in.data[z]!=TBLR_INC_N;z++){}
    if(z<OP_PGM_BUFFER_SIZE-3)
    {
        pic18f_log(__FUNCTION__,"Chip ID(Internal) : %#02X %#02X",buffer_in.data[z+3],buffer_in.data[z+2]);
    }

    pic18f_log(__FUNCTION__,"Erasing Chip ...");
    memset(buffer_out.data,0x00,buffer_out.len);
    int j=1;
    buffer_out.data[j++] = CORE_INS;
    buffer_out.data[j++] = 0x0E;
    buffer_out.data[j++] = 0x3C;
    buffer_out.data[j++] = CORE_INS;
    buffer_out.data[j++] = 0x6E;
    buffer_out.data[j++] = 0xF8;
    buffer_out.data[j++] = CORE_INS;
    buffer_out.data[j++] = 0x6A;
    buffer_out.data[j++] = 0xF7;
    if(mcu->erase_word_1 < 0x10000)
    {
        buffer_out.data[j++] = CORE_INS;
        buffer_out.data[j++] = 0x0E;
        buffer_out.data[j++] = 0x05;
        buffer_out.data[j++] = CORE_INS;
        buffer_out.data[j++] = 0x6E;
        buffer_out.data[j++] = 0xF6;
        buffer_out.data[j++] = TABLE_WRITE;
        buffer_out.data[j++] = (mcu->erase_word_1>>8)&0xFF;
        buffer_out.data[j++] = mcu->erase_word_1&0xFF;
    }
    if(mcu->erase_word_2 < 0x10000)
    {
        buffer_out.data[j++] = CORE_INS;
        buffer_out.data[j++] = 0x0E;
        buffer_out.data[j++] = 0x04;
        buffer_out.data[j++] = CORE_INS;
        buffer_out.data[j++] = 0x6E;
        buffer_out.data[j++] = 0xF6;
        buffer_out.data[j++] = TABLE_WRITE;
        buffer_out.data[j++] = (mcu->erase_word_2>>8)&0xFF;
        buffer_out.data[j++] = mcu->erase_word_2&0xFF;
    }
    buffer_out.data[j++] = CORE_INS;
    buffer_out.data[j++] = 0x00;
    buffer_out.data[j++] = 0x00;
    buffer_out.data[j++] = CORE_INS;
    buffer_out.data[j++] = 0x00;
    buffer_out.data[j++] = 0x00;
    buffer_out.data[j++] = FLUSH;

    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    return 0;
}
