#include <pic18f.h>
#include <string.h>
#include <stdint.h>

UCHAR pic18f_HV_PROG_EN_WRT[pic18f_HV_PROG_EN_WRT_SIZE]= {
    SET_PARAMETER, SET_T1T2, 0x01, 0x64,EN_VPP_VCC,
    0x00, SET_CK_D, 0x00, EN_VPP_VCC, 0x01,EN_VPP_VCC,
    0x05, WAIT_T3, CORE_INS, 0x0E, 0x3F, CORE_INS, 0x6E,
    0xF8, CORE_INS, 0x0E, 0xFF, CORE_INS, 0x6E, 0xF7,
    CORE_INS, 0x0E, 0xFE, CORE_INS, 0x6E, 0xF6,
    TBLR_INC_N, 0x02,SET_PARAMETER, SET_T3, 0x13,
    0xEC,FLUSH
};

UCHAR pic18f_LV_PROG_EN_WRT[pic18f_LV_PROG_EN_WRT_SIZE] = {
    SET_PARAMETER, SET_T1T2, 0x01, 0x64,EN_VPP_VCC,
    0x00, SET_CK_D, 0x00, EN_VPP_VCC, 0x01,EN_VPP_VCC,
    0x05, EN_VPP_VCC, 0x01,WAIT_T3, WAIT_T3, TX16,
    0x02, 0x4D, 0x43, 0x48, 0x50, EN_VPP_VCC, 0x05,
    WAIT_T3, WAIT_T3, CORE_INS, 0x0E, 0x3F, CORE_INS,
    0x6E, 0xF8, CORE_INS, 0x0E, 0xFF, CORE_INS, 0x6E,
    0xF7, CORE_INS, 0x0E, 0xFE, CORE_INS, 0x6E, 0xF6,
    TBLR_INC_N, 0x02,SET_PARAMETER, SET_T3, 0x13,
    0xEC,FLUSH
};

UCHAR pic18f_PRE_WRT[pic18f_PRE_WRT_SIZE] = {
    CORE_INS, 0x8E, 0xA6, CORE_INS, 0x9C, 0xA6,
    CORE_INS, 0x6A, 0xF8, CORE_INS, 0x6A, 0xF7,
    CORE_INS, 0x6A, 0xF6, FLUSH
};

int pic18f_write(opprog_t * op, mcu_info_t * mcu, op_mem_options_t * options)
{
    uint8_t write_flash, write_config, write_eeprom;
    write_flash = 0;
    write_config = 0;
    write_eeprom = 0;
    for(int i=0;i<options->len;i++)
    {
        if(options->types[i]==FLASH_TO_WRITE && op->memory->flash_to_write->len > 0)
        {
            write_flash = 1;
        }else if(options->types[i]==EEPROM_TO_WRITE && op->memory->eeprom_to_write->len > 0 )
        {
            write_eeprom = 1;
        }else if(options->types[i] == CONFIG_TO_WRITE && op->memory->config_to_write->len > 0)
        {
            write_config=1;
        }
    }
    if(!write_flash && !write_eeprom && !write_config)
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

    if(write_flash)
    {
        uint8_t dummy = 0xFF;
        int wbuf = mcu->pgm_page_size;
        int dim = mcu->flash_memory_size;
        int j = op->memory->flash_to_write->len;
        if(dim>op->memory->flash_to_write->len)
            dim = op->memory->flash_to_write->len;
        if(dim%wbuf)
        {
            dim+=(wbuf-(dim%wbuf));
            if(j<dim)
            {
                for(;j<dim;j++)
                    op->append_data(op,FLASH_TO_WRITE,&dummy,1);

            }
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
        j=1;
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

        pic18f_log(__FUNCTION__,"Writing Code ...");
        pic18f_progress(__FUNCTION__,0);

        memset(buffer_out.data,0x00,buffer_out.len);
        memcpy(buffer_out.data+1,pic18f_PRE_WRT,pic18f_PRE_WRT_SIZE);

        if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

        int valid,i0,k,ww;
        j=1;
        for(int i=0,k=0;i<dim;)
        {
            if(k==0)
            {
                i0=i;
                for(valid=0;!valid && i<dim;i+=valid?0:wbuf)
                {
                    for(int k2=0;k2<wbuf && !valid; k2++)
                        if(op->memory->flash_to_write->data[i+k2]<0xFF) valid = 1;
                }

                if(i>=dim) break;
                if(i>i0)
                {
                    buffer_out.data[j++]=CORE_INS;
                    buffer_out.data[j++]=0x0E;
                    buffer_out.data[j++]=(i>>16)&0xFF;
                    buffer_out.data[j++]=CORE_INS;
                    buffer_out.data[j++]=0x6E;			//MOVWF TBLPTRU
                    buffer_out.data[j++]=0xF8;
                    buffer_out.data[j++]=CORE_INS;
                    buffer_out.data[j++]=0x0E;
                    buffer_out.data[j++]=(i>>8)&0xFF;
                    buffer_out.data[j++]=CORE_INS;
                    buffer_out.data[j++]=0x6E;			//MOVWF TBLPTRH
                    buffer_out.data[j++]=0xF7;
                    buffer_out.data[j++]=CORE_INS;
                    buffer_out.data[j++]=0x0E;
                    buffer_out.data[j++]=i&0xFF;
                    buffer_out.data[j++]=CORE_INS;
                    buffer_out.data[j++]=0x6E;			//MOVWF TBLPTRL
                    buffer_out.data[j++]=0xF6;
                    buffer_out.data[j++]=FLUSH;
                    for(;j<OP_PGM_BUFFER_SIZE;j++) buffer_out.data[j]=0x00;

                    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                    j=1;
                }
            }

            if((OP_PGM_BUFFER_SIZE-4-j)<(wbuf-2-k)) ww = (OP_PGM_BUFFER_SIZE-4-j)/2;
            else ww=(wbuf-2-k)/2;

            buffer_out.data[j++] = TBLW_INC_N;
            buffer_out.data[j++] = ww;
            for(int z=0;z<ww;z++)
            {
                buffer_out.data[j++] = op->memory->flash_to_write->data[i+1];
                buffer_out.data[j++] = op->memory->flash_to_write->data[i];
                i+=2;
            }
            k+=(ww*2);

            if(k==(wbuf-2))
            {
                if(j>OP_PGM_BUFFER_SIZE-8)
                {
                    buffer_out.data[j++] = FLUSH;
                    for(;j<OP_PGM_BUFFER_SIZE;j++) buffer_out.data[j] = 0x00;

                    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                    j=1;
                }

                buffer_out.data[j++] = TBLW_PROG_INC;
                buffer_out.data[j++] = op->memory->flash_to_write->data[i+1];
                buffer_out.data[j++] = op->memory->flash_to_write->data[i];
                buffer_out.data[j++] = 1000>>8;//1ms
                buffer_out.data[j++] = 1000&0xFF;
                buffer_out.data[j++] = FLUSH;
                i+=2;
                k=0;

                if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                j=1;
                pic18f_progress(__FUNCTION__,i*100/dim);
            }
        }

        pic18f_progress(__FUNCTION__,100);
    }


    if(write_config && op->memory->config_to_write->len == 22)//ID
    {
        pic18f_log(__FUNCTION__,"Writing IDs ...");
        int j=1;
        memset(buffer_out.data,0x00,buffer_out.len);
        buffer_in.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x8E;
        buffer_out.data[j++]=0xA6;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x9C;
        buffer_out.data[j++]=0xA6;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x0E;			//TBLPTRU	ID 0x200000
        buffer_out.data[j++]=0x20;			//TBLPTRU	ID 0x200000
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x6E;			//TBLPTRU
        buffer_out.data[j++]=0xF8;			//TBLPTRU
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x6A;			//TBLPTRH
        buffer_out.data[j++]=0xF7;			//TBLPTRH
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x6A;			//TBLPTRL
        buffer_out.data[j++]=0xF6;			//TBLPTRL
        buffer_out.data[j++]=TBLW_INC_N;
        buffer_out.data[j++]=3;
        int i;
        for(i=0;i<3;i++)
        {
            buffer_out.data[j++] = op->memory->config_to_write->data[i*2+1];
            buffer_out.data[j++] = op->memory->config_to_write->data[i*2];
        }
        buffer_out.data[j++] = TBLW_PROG;
        buffer_out.data[j++] = op->memory->config_to_write->data[i*2+1];
        buffer_out.data[j++] = op->memory->config_to_write->data[i*2];
        buffer_out.data[j++] = 1000>>8;
        buffer_out.data[j++] = 1000&0xFF;
        buffer_out.data[j++] = FLUSH;

        if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    }

    if(write_eeprom)
    {
        pic18f_log(__FUNCTION__,"Writing EEPROM");

        int errEE=0;
        int j =1;
        memset(buffer_out.data,0x00,buffer_out.len);
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x9E;			//EEPGD=0
        buffer_out.data[j++]=0xA6;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x9C;			//CFGS=0
        buffer_out.data[j++]=0xA6;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x84;			//WREN=1
        buffer_out.data[j++]=0xA6;

        for(int i =0;i<op->memory->eeprom_to_write->len && errEE<= MAX_ERR;i++)
        {
            if(op->memory->eeprom_to_write->data[i] != 0xFF)
            {
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x0E;
                buffer_out.data[j++]=i&0xFF;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x6E;
                buffer_out.data[j++]=0xA9;


                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x0E;
                buffer_out.data[j++]=(i>>8)&0xFF;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x6E;
                buffer_out.data[j++]=0xAA;


                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x0E;
                buffer_out.data[j++]=op->memory->eeprom_to_write->data[i];
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x6E;
                buffer_out.data[j++]=0xA8;
                //One thing skipped here for now

                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x82;
                buffer_out.data[j++]=0xA6;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=WAIT_T3;		//write delay
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x80;			//RD=1
                buffer_out.data[j++]=0xA6;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x50;			//MOVF EEDATA,w
                buffer_out.data[j++]=0xA8;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x6E;			//MOVWF TABLAT
                buffer_out.data[j++]=0xF5;
                buffer_out.data[j++]=CORE_INS;
                buffer_out.data[j++]=0x00;			//NOP
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=SHIFT_TABLAT;
                buffer_out.data[j++]=FLUSH;

                for(;j<OP_PGM_BUFFER_SIZE;j++) buffer_out.data[j]=0x00;

                if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                j=1;

                pic18f_progress(__FUNCTION__,i*100/op->memory->eeprom_to_write->len);
            }
        }
    }

    if(write_config)
    {
        int config_pos = 0;
        if(op->memory->config_to_write->len == 22) config_pos = 8;
        else config_pos = 0;

        pic18f_log(__FUNCTION__,"Writing Configurations");

        memset(buffer_out.data,0x00,buffer_out.len);
        int j =1;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x8E;
        buffer_out.data[j++]=0xA6;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x8C;
        buffer_out.data[j++]=0xA6;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x0E;			//CONFIG 0x300000
        buffer_out.data[j++]=0x30;
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x6E;			//TBLPTRU
        buffer_out.data[j++]=0xF8;			//TBLPTRU
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x6A;			//TBLPTRH
        buffer_out.data[j++]=0xF7;			//TBLPTRH
        buffer_out.data[j++]=CORE_INS;
        buffer_out.data[j++]=0x6A;			//TBLPTRL
        buffer_out.data[j++]=0xF6;			//TBLPTRL
        for(int i=0;i<14;i++)
        {
            if(op->memory->config_to_write->data[i] < 0xFF)
            {
                buffer_out.data[j++]=TBLW_PROG;
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=op->memory->config_to_write->data[i];
                buffer_out.data[j++]=5000>>8;
                buffer_out.data[j++]=5000&0xFF;
            }
            buffer_out.data[j++]=CORE_INS;
            buffer_out.data[j++]=0x2A;
            buffer_out.data[j++]=0xF6;
            i++;
            if(op->memory->config_to_write->data[i] < 0xFF)
            {
                buffer_out.data[j++]=TBLW_PROG;
                buffer_out.data[j++]=op->memory->config_to_write->data[i];
                buffer_out.data[j++]=0x00;
                buffer_out.data[j++]=5000>>8;
                buffer_out.data[j++]=5000&0xFF;
            }
            buffer_out.data[j++]=CORE_INS;
            buffer_out.data[j++]=0x2A;
            buffer_out.data[j++]=0xF6;
            buffer_out.data[j++]=FLUSH;

            for(;j<OP_PGM_BUFFER_SIZE;j++)buffer_out.data[j]=0x00;

            if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
            if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

            j=1;
        }
    }

    memset(buffer_out.data,0x00,buffer_out.len);
    int j =1;
    buffer_out.data[j++]=SET_PARAMETER;
    buffer_out.data[j++]=SET_T3;
    buffer_out.data[j++]=2000>>8;
    buffer_out.data[j++]=2000&0xff;
    buffer_out.data[j++]=EN_VPP_VCC;		//VDD
    buffer_out.data[j++]=1;
    buffer_out.data[j++]=EN_VPP_VCC;		//0
    buffer_out.data[j++]=0x0;
    buffer_out.data[j++]=SET_CK_D;
    buffer_out.data[j++]=0x0;
    buffer_out.data[j++]=FLUSH;

    if((op->send(op,&buffer_out)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; pic18f_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    return 0;
}
