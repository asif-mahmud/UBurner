#include <pic18f.h>
#include <string.h>
#include <stdint.h>

UCHAR pic18f_HV_PROG_EN[pic18f_HV_PROG_EN_SIZE] = {
    SET_PARAMETER, SET_T1T2, 0x01, 0x64, SET_PARAMETER,
    SET_T3, 0x07, 0xD0, EN_VPP_VCC, 0x00, SET_CK_D,
    0x00, EN_VPP_VCC, 0x01, NOP, EN_VPP_VCC, 0x05,
    WAIT_T3, CORE_INS, 0x0E, 0x3F, CORE_INS, 0x6E,
    0xF8, CORE_INS, 0x0E, 0xFF, CORE_INS, 0x6E, 0xF7,
    CORE_INS, 0x0E, 0xFE, CORE_INS, 0x6E, 0xF6,
    TBLR_INC_N, 0x02, CORE_INS, 0x6A, 0xF8,
    CORE_INS, 0x6A, 0xF7, CORE_INS, 0x6A, 0xF6, FLUSH
};

UCHAR pic18f_LV_PROG_EN[pic18f_LV_PROG_EN_SIZE] = {
    SET_PARAMETER, SET_T1T2, 0x01, 0x64, SET_PARAMETER,
    SET_T3, 0x07, 0xD0, EN_VPP_VCC, 0x00, SET_CK_D,
    0x00, EN_VPP_VCC, 0x01, NOP, EN_VPP_VCC, 0x05,
    EN_VPP_VCC, 0x01, WAIT_T3, WAIT_T3, TX16, 0x02,
    0x4D, 0x43, 0x48, 0x50, EN_VPP_VCC, 0x05, WAIT_T3,
    WAIT_T3, CORE_INS, 0x0E, 0x3F, CORE_INS, 0x6E,
    0xF8, CORE_INS, 0x0E, 0xFF, CORE_INS, 0x6E, 0xF7,
    CORE_INS, 0x0E, 0xFE, CORE_INS, 0x6E, 0xF6,
    TBLR_INC_N, 0x02, CORE_INS, 0x6A, 0xF8,
    CORE_INS, 0x6A, 0xF7, CORE_INS, 0x6A, 0xF6, FLUSH
};

UCHAR pic18f_READ_CONFIG[pic18f_READ_CONFIG_SIZE] = {
    CORE_INS, 0x0E, 0x20, CORE_INS, 0x6E, 0xF8,
    CORE_INS, 0x6A, 0xF7, CORE_INS, 0x6A, 0xF6,
    TBLR_INC_N, 0x08, CORE_INS, 0x0E, 0x30,
    CORE_INS, 0x6E, 0xF8, CORE_INS, 0x6A, 0xF7,
    CORE_INS, 0x6A, 0xF6, TBLR_INC_N, 0x0E,
    FLUSH
};

UCHAR pic18f_EXIT[pic18f_EXIT_SIZE]= {
    EN_VPP_VCC, 0x01, EN_VPP_VCC, 0x0,
    SET_CK_D, 0x00, FLUSH
};

/**
 * only J versions require low voltage programming
 * @brief pic18f_read
 * @param op
 * @param mcu
 * @param options
 * @return
 */
int pic18f_read(opprog_t *op, mcu_info_t *mcu, op_mem_options_t * options)
{
    uint8_t read_flash, read_config, read_eeprom;
    read_flash = 0;
    read_config = 0;
    read_eeprom = 0;
    for(int i=0;i<options->len;i++)
    {
        if(options->types[i]==FLASH_READ)
        {
            op->clear_memory(op,FLASH_READ);
            read_flash = 1;
        }else if(options->types[i]==EEPROM_READ)
        {
            op->clear_memory(op,EEPROM_READ);
            read_eeprom = 1;
        }else if(options->types[i] == CONFIG_READ)
        {
            op->clear_memory(op,CONFIG_READ);
            read_config=1;
        }
    }
    if(!read_flash && !read_eeprom && !read_config)
    {
        pic18f_log(__FUNCTION__,"Nothing to do.");
        return 0;
    }

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
    if(lv_prog) memcpy(buffer_out.data+1,pic18f_LV_PROG_EN,pic18f_LV_PROG_EN_SIZE);
    else memcpy(buffer_out.data+1,pic18f_HV_PROG_EN,pic18f_HV_PROG_EN_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    for(ret = 1;(buffer_in.data[ret]!=TBLR_INC_N) && (ret<OP_PGM_BUFFER_SIZE);ret++){}
    pic18f_log(__FUNCTION__,"Chip ID(Internal) : %#X %#X",buffer_in.data[ret+2],buffer_in.data[ret+3]);

    if(read_flash)
    {
        int i,j,cnt=0;
        pic18f_progress(__FUNCTION__,0);
        for(i=0,j=1;i<mcu->flash_memory_size;i+=(OP_PGM_BUFFER_SIZE-4))
        {
            memset(buffer_out.data,0x00,buffer_out.len);
            buffer_out.data[j++] = TBLR_INC_N;
            buffer_out.data[j++] = (i<(mcu->flash_memory_size-(OP_PGM_BUFFER_SIZE-4)))?(OP_PGM_BUFFER_SIZE-4):(mcu->flash_memory_size-i);
            buffer_out.data[j++] = FLUSH;

            if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
            if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

            if(buffer_in.data[0] == TBLR_INC_N)
            {
                for(int z=2;z<(buffer_in.data[1]+2)&&(z<OP_PGM_BUFFER_SIZE);z++)
                {
                    op->append_data(op,FLASH_READ,&buffer_in.data[z],1);
                    cnt++;
                }
            }

            pic18f_progress(__FUNCTION__,i*100/mcu->flash_memory_size);
            j=1;
        }
        pic18f_progress(__FUNCTION__,100);
        pic18f_log(__FUNCTION__,"Total %d b Flash read",cnt);
        if(cnt != mcu->flash_memory_size)
        {
            pic18f_log(__FUNCTION__,"Error Reading Flash data");
            return -1;
        }else
            pic18f_log(__FUNCTION__,"show_flash_read");
    }

    if(read_config)
    {
        pic18f_log(__FUNCTION__,"Reading Config Bytes");

        memset(buffer_out.data,0x00,buffer_out.len);
        memcpy(buffer_out.data+1,pic18f_READ_CONFIG,pic18f_READ_CONFIG_SIZE);

        if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

        int pos;
        for(pos = 0;(buffer_in.data[pos]!=TBLR_INC_N)&&(pos<OP_PGM_BUFFER_SIZE<28);pos++){}
        if(pos<(OP_PGM_BUFFER_SIZE-28))
        {
            for(int i=0;i<8;i++)
                op->append_data(op,CONFIG_READ,&buffer_in.data[pos+i+2],1);
            for(int i=8;i<(14+8);i++)
                op->append_data(op,CONFIG_READ,&buffer_in.data[pos+i+8],1);
        }

        pic18f_log(__FUNCTION__,"show_config_read");
        pic18f_log(__FUNCTION__,"Total %d b Config read.",op->memory->config_read->len);
    }

    if(read_eeprom)
    {
        pic18f_log(__FUNCTION__,"Reading EEPROM");
        pic18f_progress(__FUNCTION__,0);

        memset(buffer_out.data,0x00,buffer_out.len);
        int j =1;
        int cnt = 0;

        buffer_out.data[j++] = CORE_INS;
        buffer_out.data[j++] = 0x9E;
        buffer_out.data[j++] = 0xA6;
        buffer_out.data[j++] = CORE_INS;
        buffer_out.data[j++] = 0x9C;
        buffer_out.data[j++] = 0xA6;
        for(int i = 0;i<mcu->data_memory_size;i++)
        {
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x0E;
            buffer_out.data[j++] = i&0xFF;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x6E;
            buffer_out.data[j++] = 0xA9;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x0E;
            buffer_out.data[j++] = (i>>8)&0xFF;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x6E;
            buffer_out.data[j++] = 0xAA;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x80;
            buffer_out.data[j++] = 0xA6;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x50;
            buffer_out.data[j++] = 0xA8;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x6E;
            buffer_out.data[j++] = 0xF5;
            buffer_out.data[j++] = CORE_INS;
            buffer_out.data[j++] = 0x00;
            buffer_out.data[j++] = 0x00;
            buffer_out.data[j++] = SHIFT_TABLAT;
            if(j>OP_PGM_BUFFER_SIZE-26 || i==mcu->data_memory_size-1)
            {
                buffer_out.data[j++] = FLUSH;
                for(;j<OP_PGM_BUFFER_SIZE;j++)buffer_out.data[j] = 0x00;

                if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                for(int z=0;z<(OP_PGM_BUFFER_SIZE-1);z++)
                {
                    if(buffer_in.data[z] == SHIFT_TABLAT)
                    {
                        op->append_data(op,EEPROM_READ,&buffer_in.data[z+1],1);
                        z+=8;
                        cnt++;
                    }
                }
                pic18f_progress(__FUNCTION__,i*100/mcu->data_memory_size);

                j=1;
            }

        }

        pic18f_progress(__FUNCTION__,100);
        pic18f_log(__FUNCTION__,"Total %d b EEPROM read",cnt);
        if(cnt != mcu->data_memory_size)
        {
            pic18f_log(__FUNCTION__,"Error reading EEPROM data");
            return -1;
        }else
        {
             pic18f_log(__FUNCTION__,"show_eeprom_read");
        }
    }

    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,pic18f_EXIT,pic18f_EXIT_SIZE);
    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    return 0;
}

