#include <atmel.h>
#include <string.h>

UCHAR at_LV_CLK[at_LV_CLK_SIZE] = {
    SET_PARAMETER, SET_T3, 78,32,
    VREG_DIS, EN_VPP_VCC, 0x00,
    SPI_INIT, 0x00, CLOCK_GEN, 0x03,
    CLOCK_GEN, 0xFF, EXT_PORT, 0x00,
    0x00, WAIT_T3, EN_VPP_VCC, 0x01,
    WAIT_T3, FLUSH
};

UCHAR at_LV_PGM_EN[at_LV_PGM_EN_SIZE] = {
    CLOCK_GEN, 0xFF, EXT_PORT, 0x00,
    RST, EN_VPP_VCC,0x05, EXT_PORT, 0x00,
    0x00, EN_VPP_VCC, 0x01, CLOCK_GEN, 0x04,
    WAIT_T3, SPI_WRITE, 0x02,
    0xAC, 0x53, SPI_READ, 0x02, FLUSH
};

UCHAR at_LV_SPI_EXIT[at_LV_SPI_EXIT_SIZE] = {
    EN_VPP_VCC, 0x00, SPI_INIT, 0xFF,
    CLOCK_GEN, 0xFF, EXT_PORT, 0x00, 0x00,
    FLUSH
};

UCHAR at_LV_SIG_READ[at_LV_SIG_READ_SIZE] = {
    SPI_WRITE, 0x03, 0x30, 0x00, 0x00,
    SPI_READ, 0x01, SPI_WRITE, 0x03, 0x30,
    0x00, 0x01, SPI_READ, 0x01, SPI_WRITE,
    0x03, 0x30, 0x00, 0x02, SPI_READ, 0x01
};

UCHAR at_LV_LOCK_READ[at_LV_LOCK_READ_SIZE] = {
    SPI_WRITE, 0x03, 0x58, 0x00, 0x00,
    SPI_READ, 0x01
};

UCHAR at_LV_LFUSE_READ[at_LV_LFUSE_READ_SIZE] = {
    SPI_WRITE, 0x03, 0x50, 0x00, 0x00,
    SPI_READ, 0x01
};

UCHAR at_LV_HFUSE_READ[at_LV_HFUSE_READ_SIZE] = {
    SPI_WRITE, 0x03, 0x58, 0x08, 0x00,
    SPI_READ, 0x01
};

UCHAR at_LV_EFUSE_READ[at_LV_EFUSE_READ_SIZE] = {
    SPI_WRITE, 0x03, 0x50, 0x08, 0x00,
    SPI_READ, 0x01
};

UCHAR at_LV_CAL_READ[at_LV_CAL_READ_SIZE] = {
    SPI_WRITE, 0x03, 0x38, 0x00, 0x00,
    SPI_READ, 0x01, SPI_WRITE, 0x03, 0x38,
    0x00, 0x01, SPI_READ, 0x01, SPI_WRITE,
    0x03, 0x38, 0x00, 0x02, SPI_READ, 0x01,
    SPI_WRITE, 0x03, 0x38, 0x00, 0x03,
    SPI_READ, 0x01, FLUSH
};

UCHAR at_LV_EXIT[at_LV_EXIT_SIZE] = {
    CLOCK_GEN, 0xFF, SPI_WRITE, 0x01, 0x00,
    EXT_PORT, 0x00, RST, EN_VPP_VCC,0x05,FLUSH
};

int atmel_read(opprog_t *op, mcu_info_t *mcu, op_mem_options_t * options)
{
    UCHAR read_flash,read_eeprom, read_cal, read_lfuse, read_hfuse, read_efuse, read_lock;
    int ret = 0,cnt,cnt2;
    UCHAR dummy = 0x00;
    op_buffer_t buffer_out, buffer_in;
    buffer_out.len = OP_PGM_BUFFER_SIZE;
    buffer_out.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
    buffer_in.len = OP_PGM_BUFFER_SIZE;
    buffer_in.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));

    read_flash = 0;
    read_eeprom = 0;
    read_cal = 0;
    read_lfuse = 0;
    read_hfuse = 0;
    read_efuse = 0;
    read_lock = 0;
    for(int i =0; i<options->len;i++)
    {
        if(options->types[i] == FLASH_READ)
        {
            op->clear_memory(op,FLASH_READ);
            at_log(__FUNCTION__,"initialized FLASH_READ ...");
            read_flash = 1;
        }else if(options->types[i] ==EEPROM_READ)
        {
            op->clear_memory(op,EEPROM_READ);
            at_log(__FUNCTION__,"initialized EEPROM_READ ...");
            read_eeprom = 1;
        }else if(options->types[i] ==AVR_CAL)
        {
            read_cal = 1;
        }else if(options->types[i] ==AVR_LFUSE_READ)
        {
            read_lfuse = 1;
        }else if(options->types[i] ==AVR_HFUSE_READ)
        {
            read_hfuse = 1;
        }else if(options->types[i] ==AVR_EFUSE_READ)
        {
            read_efuse = 1;
        }else if(options->types[i] ==AVR_LOCK_READ)
        {
            read_lock = 1;
        }
    }

    if( !read_flash && !read_cal && !read_lfuse && !read_hfuse && !read_efuse && !read_eeprom)
        return 0;//nothing to do here

    at_log(__FUNCTION__,"Chip : %s F_SIZE : %d D_SIZE : %d",mcu->name,mcu->flash_memory_size,mcu->data_memory_size);
    at_log(__FUNCTION__,"initializing memories ...");
    op->clear_memory(op,CONFIG_READ);
    at_log(__FUNCTION__,"initialized memories ...");

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

    at_log(__FUNCTION__,"reading signature ...");
    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,at_LV_SIG_READ,at_LV_SIG_READ_SIZE);
    cnt = at_LV_SIG_READ_SIZE +2;

    if(read_lock)
    {
        memcpy(buffer_out.data+cnt,at_LV_LOCK_READ,at_LV_LOCK_READ_SIZE);
        cnt += at_LV_LOCK_READ_SIZE;
    }
    if(read_lfuse)
    {
        memcpy(buffer_out.data+cnt,at_LV_LFUSE_READ,at_LV_LFUSE_READ_SIZE);
        cnt += at_LV_LFUSE_READ_SIZE;
    }
    if(read_hfuse)
    {
        memcpy(buffer_out.data+cnt,at_LV_HFUSE_READ,at_LV_HFUSE_READ_SIZE);
        cnt += at_LV_HFUSE_READ_SIZE;
    }
    if(read_efuse)
    {
        memcpy(buffer_out.data+cnt,at_LV_EFUSE_READ,at_LV_EFUSE_READ_SIZE);
        cnt += at_LV_EFUSE_READ_SIZE;
    }

    buffer_out.data[cnt+1] = FLUSH;

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    for(cnt = 1;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
    op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
    for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
    op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
    for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
    op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);

    at_log(__FUNCTION__,"%#2X %#2X %#2X",op->memory->config_read->data[AVR_ID_POS],op->memory->config_read->data[AVR_ID_POS+1],op->memory->config_read->data[AVR_ID_POS+2]);

    if(read_lock)
    {
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        at_log(__FUNCTION__,"LOCK %#2X",op->memory->config_read->data[AVR_LOCK_POS]);
    }else
        op->append_data(op,CONFIG_READ,&dummy,1);
    if(read_lfuse)
    {
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        at_log(__FUNCTION__,"LFUSE %#2X",op->memory->config_read->data[AVR_LFUSE_POS]);
    }else
        op->append_data(op,CONFIG_READ,&dummy,1);
    if(read_hfuse)
    {
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        at_log(__FUNCTION__,"HFUSE %#2X",op->memory->config_read->data[AVR_HFUSE_POS]);
    }else
        op->append_data(op,CONFIG_READ,&dummy,1);
    if(read_efuse)
    {
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        at_log(__FUNCTION__,"EFUSE %#2X",op->memory->config_read->data[AVR_EFUSE_POS]);
    }else
        op->append_data(op,CONFIG_READ,&dummy,1);

    at_log(__FUNCTION__,"show_config_read");
    if(read_cal)//it should always be read
    {
        memset(buffer_out.data,0x00,buffer_out.len);
        memcpy(buffer_out.data+1,at_LV_CAL_READ,at_LV_CAL_READ_SIZE);

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

        for(cnt = 1;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);
        for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
        op->append_data(op,CONFIG_READ,&buffer_in.data[cnt+2],1);

        at_log(__FUNCTION__,"%#2X %#2X %#2X %#2X",
               op->memory->config_read->data[AVR_CAL_POS],
               op->memory->config_read->data[AVR_CAL_POS+1],
               op->memory->config_read->data[AVR_CAL_POS+2],
               op->memory->config_read->data[AVR_CAL_POS+3]);
    }

    if(read_flash)
    {
        at_log(__FUNCTION__,"reading FLASH data");
        at_progress(__FUNCTION__,0);

        int cnt3 = (OP_PGM_BUFFER_SIZE-5)/2;
        int flash_read_cnt = 0;
        for(cnt=0,cnt2=1;cnt<mcu->flash_memory_size;cnt+=(cnt3*2))
        {
            memset(buffer_out.data,0x00,buffer_out.len);
            buffer_out.data[cnt2++] = AT_READ_DATA;
            buffer_out.data[cnt2++] = (cnt<(mcu->flash_memory_size - 2*cnt3))?cnt3:(mcu->flash_memory_size-cnt)/2;
            buffer_out.data[cnt2++] = cnt>>9;
            buffer_out.data[cnt2++] = cnt>>1;
            buffer_out.data[cnt2++] = FLUSH;

            if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
            if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

            //at_log(__FUNCTION__,"%#2X %#2X",buffer_in.data[0],buffer_in.data[1]);
            if(buffer_in.data[0] == AT_READ_DATA)
            {
                for(int i=2;i<buffer_in.data[1]*2+2 && i<OP_PGM_BUFFER_SIZE; i++)
                {
                    flash_read_cnt++;
                    op->append_data(op,FLASH_READ,&buffer_in.data[i],1);
                }
            }

            at_progress(__FUNCTION__,cnt*100/mcu->flash_memory_size);
            cnt2 = 1;
        }
        at_log(__FUNCTION__,"Total %d b FLASH read",flash_read_cnt);
        //at_log(__FUNCTION__,"First byte : %#x",op->memory->flash_read->data[0]);
        at_progress(__FUNCTION__,100);

        if(flash_read_cnt != mcu->flash_memory_size)
        {
            at_log(__FUNCTION__,"Error reading FLASH");
        }else
        {
            at_log(__FUNCTION__,"show_flash_read");
        }
    }

    if(read_eeprom)
    {
        at_log(__FUNCTION__ ,"reading EEPROM data");
        at_progress(__FUNCTION__,0);

        int eeprom_read_cnt = 0;
        for(cnt=0,cnt2=1;cnt < mcu->data_memory_size; cnt++)
        {
            buffer_out.data[cnt2++] = SPI_WRITE;
            buffer_out.data[cnt2++] = 0x03;
            buffer_out.data[cnt2++] = 0xA0;
            buffer_out.data[cnt2++] = cnt>>8;
            buffer_out.data[cnt2++] = cnt;
            buffer_out.data[cnt2++] = SPI_READ;
            buffer_out.data[cnt2++] = 0x01;
            if((cnt2>(OP_PGM_BUFFER_SIZE-9)) || (cnt == (mcu->data_memory_size-1)) )
            {
                buffer_out.data[cnt2++] = FLUSH;
                memset(buffer_out.data+cnt2,0x00,buffer_out.len-cnt2);


                if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                for(int i=0;i<(OP_PGM_BUFFER_SIZE-2);i++)
                {
                    if(buffer_in.data[i] == SPI_READ && buffer_in.data[i+1]==0x01)
                    {
                        op->append_data(op,EEPROM_READ,&buffer_in.data[i+2],1);
                        eeprom_read_cnt++;
                        i+=3;
                    }
                }
                at_progress(__FUNCTION__,eeprom_read_cnt*100/mcu->data_memory_size);
                cnt2 = 1;
            }
        }
        at_log(__FUNCTION__,"Total %d b EEPROM read",eeprom_read_cnt);
        at_progress(__FUNCTION__,100);

        if(eeprom_read_cnt != mcu->data_memory_size)
        {
            at_log(__FUNCTION__,"Error reading EEPROM");
        }else
        {
            at_log(__FUNCTION__,"show_eeprom_read");
        }
    }


    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,at_LV_EXIT,at_LV_EXIT_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    return 0;
}

