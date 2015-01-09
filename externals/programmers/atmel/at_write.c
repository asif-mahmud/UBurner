#include <atmel.h>
#include <string.h>

int atmel_write(opprog_t * op, mcu_info_t * mcu, op_mem_options_t * options)
{
    UCHAR write_flash,write_eeprom, write_lfuse, write_hfuse, write_efuse, write_lock;
    int ret = 0,cnt,cnt2;
    UCHAR dummy = 0xFF;
    int page_size = at_get_page_size(mcu);
    op_buffer_t buffer_out, buffer_in;
    buffer_out.len = OP_PGM_BUFFER_SIZE;
    buffer_out.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
    buffer_in.len = OP_PGM_BUFFER_SIZE;
    buffer_in.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));

    write_flash = 0;
    write_eeprom = 0;
    write_lfuse = 0;
    write_hfuse = 0;
    write_efuse = 0;
    write_lock = 0;
    for(int i =0; i<options->len;i++)
    {
        if(options->types[i] == FLASH_TO_WRITE && op->memory->flash_to_write->len > 0)
        {
            at_log(__FUNCTION__,"initialized FLASH_TO_WRITE ...");
            write_flash = 1;
        }else if(options->types[i] ==EEPROM_TO_WRITE && op->memory->eeprom_to_write->len > 0)
        {
            at_log(__FUNCTION__,"initialized EEPROM_TO_WRITE ...");
            write_eeprom = 1;
        }else if(options->types[i] ==AVR_LFUSE_WRITE && op->memory->config_to_write->len == AVR_CONFIG_SIZE)
        {
            write_lfuse = 1;
        }else if(options->types[i] ==AVR_HFUSE_WRITE && op->memory->config_to_write->len == AVR_CONFIG_SIZE)
        {
            write_hfuse = 1;
        }else if(options->types[i] ==AVR_EFUSE_WRITE && op->memory->config_to_write->len == AVR_CONFIG_SIZE)
        {
            write_efuse = 1;
        }else if(options->types[i] ==AVR_LOCK_WRITE && op->memory->config_to_write->len == AVR_CONFIG_SIZE)
        {
            write_lock = 1;
        }
    }

    if( !write_flash && !write_lfuse && !write_hfuse && !write_efuse && !write_eeprom)
        return 0;//nothing to do here

    at_log(__FUNCTION__,"Chip : %s F_SIZE : %d D_SIZE : %d",mcu->name,mcu->flash_memory_size,mcu->data_memory_size);
    at_log(__FUNCTION__,"Page : %d bytes",page_size);

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

    buffer_out.data[cnt] = FLUSH;

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    for(cnt = 1;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
    at_log(__FUNCTION__,"%#2x",buffer_in.data[cnt+2]);
    for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
    at_log(__FUNCTION__,"%#2x",buffer_in.data[cnt+2]);
    for(cnt +=3;(cnt<(OP_PGM_BUFFER_SIZE-2)) && (buffer_in.data[cnt] != SPI_READ);cnt++);
    at_log(__FUNCTION__,"%#2x",buffer_in.data[cnt+2]);

    //erase first
    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,at_LV_ERASE,at_LV_ERASE_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

    at_log(__FUNCTION__,"Chip erased");

    if( write_flash )
    {
        at_log(__FUNCTION__,"Writing FLASH data");
        at_progress(__FUNCTION__,0);

        int err_cnt = 0;

        if(page_size)
        {
            int dim = mcu->flash_memory_size;
            if(dim >= op->memory->flash_to_write->len)
                dim = op->memory->flash_to_write->len;
            else
            {
                at_log(__FUNCTION__,"Given Data Size mismatched !");
                return -1;
            }
            if(op->memory->flash_to_write->len%(page_size*2))
            {
                int j = op->memory->flash_to_write->len;
                dim = (j/(page_size*2)+1)*page_size*2;
                for(;j<dim;j++)
                    op->append_data(op,FLASH_TO_WRITE,&dummy,1);
            }

            for(cnt=0;cnt<dim;cnt+=page_size*2)
            {
                UCHAR valid =0;
                for(int x=cnt;x<(cnt+page_size*2);x++) if(op->memory->flash_to_write->data[x] < 0xFF) valid=1;
                if(valid)
                {
                    int w = 0;
                    for(int k=0,cnt2=1,valid=0;k<page_size;k+=w)
                    {
                        w=(page_size-k)<(OP_PGM_BUFFER_SIZE-6)/2?(page_size-k):(OP_PGM_BUFFER_SIZE-6)/2;
                        memset(buffer_out.data,0x00,buffer_out.len);
                        buffer_out.data[cnt2++] = AT_LOAD_DATA;
                        buffer_out.data[cnt2++] = w;
                        buffer_out.data[cnt2++] = k>>8;
                        buffer_out.data[cnt2++] = k;
                        for(int z=0;z<(w*2);z++)
                            buffer_out.data[cnt2++] = op->memory->flash_to_write->data[cnt+k*2+z];
                        buffer_out.data[cnt2++] = FLUSH;
                        cnt2=1;
                        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                    }
                    memset(buffer_out.data,0x00,buffer_out.len);
                    buffer_out.data[cnt2++] = SPI_WRITE;
                    buffer_out.data[cnt2++] = 0x04;
                    buffer_out.data[cnt2++] = 0x4C;
                    buffer_out.data[cnt2++] = cnt>>9;
                    buffer_out.data[cnt2++] = cnt>>1;
                    buffer_out.data[cnt2++] = 0x00;
                    buffer_out.data[cnt2++] = WAIT_T3;
                    buffer_out.data[cnt2++] = FLUSH;

                    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                    at_progress(__FUNCTION__,cnt*100/op->memory->flash_to_write->len);
                    cnt2=1;
                }
            }
        }

        at_progress(__FUNCTION__,100);
        at_log(__FUNCTION__,"Error CNT  : %d",err_cnt);
    }

    if(write_eeprom)
    {
        at_log(__FUNCTION__,"Writing EEPROM");
        at_progress(__FUNCTION__,0);
        for(cnt=0,cnt2=1;cnt<mcu->data_memory_size;cnt++)
        {
            if(op->memory->eeprom_to_write->data[cnt] != 0xFF)
            {
                memset(buffer_out.data,0x00,buffer_out.len);
                buffer_out.data[cnt2++]=SPI_WRITE;		//Write EEPROM memory
                buffer_out.data[cnt2++]=4;
                buffer_out.data[cnt2++]=0xC0;
                buffer_out.data[cnt2++]=cnt>>8;
                buffer_out.data[cnt2++]=cnt;
                buffer_out.data[cnt2++]=op->memory->eeprom_to_write->data[cnt];
                buffer_out.data[cnt2++]=WAIT_T3;		//5ms
                buffer_out.data[cnt2++]=WAIT_T3;		//5ms
                buffer_out.data[cnt2++]=SPI_WRITE;		//Read EEPROM memory
                buffer_out.data[cnt2++]=3;
                buffer_out.data[cnt2++]=0xA0;
                buffer_out.data[cnt2++]=cnt>>8;
                buffer_out.data[cnt2++]=cnt;
                buffer_out.data[cnt2++]=SPI_READ;
                buffer_out.data[cnt2++]=1;
                buffer_out.data[cnt2++]=FLUSH;
                cnt2 = 1;

                if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
                if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}

                at_progress(__FUNCTION__,cnt*100/mcu->data_memory_size);
            }
        }
        at_progress(__FUNCTION__,100);
    }

    if(write_lfuse)
    {
        int j =1;
        at_log(__FUNCTION__,"Writing Low Fuse");
        memset(buffer_out.data,0x00,buffer_out.len);
        buffer_out.data[j++]=SPI_WRITE;		//Write fuse
        buffer_out.data[j++]=4;
        buffer_out.data[j++]=0xAC;
        buffer_out.data[j++]=0xA0;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=op->memory->config_to_write->data[AVR_LFUSE_POS];
        buffer_out.data[j++]=WAIT_T3;		//9ms
        buffer_out.data[j++]=SPI_WRITE;
        buffer_out.data[j++]=3;
        buffer_out.data[j++]=0x50;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=SPI_READ;
        buffer_out.data[j++]=1;
        buffer_out.data[j++]=FLUSH;

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    }

    if(write_hfuse)
    {
        int j =1;
        at_log(__FUNCTION__,"Writing High Fuse");
        memset(buffer_out.data,0x00,buffer_out.len);
        buffer_out.data[j++]=SPI_WRITE;		//Write fuse
        buffer_out.data[j++]=4;
        buffer_out.data[j++]=0xAC;
        buffer_out.data[j++]=0xA8;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=op->memory->config_to_write->data[AVR_HFUSE_POS];
        buffer_out.data[j++]=WAIT_T3;		//9ms
        buffer_out.data[j++]=SPI_WRITE;
        buffer_out.data[j++]=3;
        buffer_out.data[j++]=0x58;
        buffer_out.data[j++]=8;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=SPI_READ;
        buffer_out.data[j++]=1;
        buffer_out.data[j++]=FLUSH;

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    }

    if(write_efuse)
    {
        int j =1;
        at_log(__FUNCTION__,"Writing Extended Fuse");
        memset(buffer_out.data,0x00,buffer_out.len);
        buffer_out.data[j++]=SPI_WRITE;		//Write fuse
        buffer_out.data[j++]=4;
        buffer_out.data[j++]=0xAC;
        buffer_out.data[j++]=0xA4;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=op->memory->config_to_write->data[AVR_EFUSE_POS];
        buffer_out.data[j++]=WAIT_T3;		//9ms
        buffer_out.data[j++]=SPI_WRITE;
        buffer_out.data[j++]=3;
        buffer_out.data[j++]=0x50;
        buffer_out.data[j++]=8;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=SPI_READ;
        buffer_out.data[j++]=1;
        buffer_out.data[j++]=FLUSH;

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    }

    if(write_lock)
    {
        int j =1;
        at_log(__FUNCTION__,"Writing Lock byte");
        memset(buffer_out.data,0x00,buffer_out.len);
        buffer_out.data[j++]=SPI_WRITE;		//Write fuse
        buffer_out.data[j++]=4;
        buffer_out.data[j++]=0xAC;
        buffer_out.data[j++]=0xE0;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=op->memory->config_to_write->data[AVR_LOCK_POS];
        buffer_out.data[j++]=WAIT_T3;		//9ms
        buffer_out.data[j++]=SPI_WRITE;
        buffer_out.data[j++]=3;
        buffer_out.data[j++]=0x58;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=0;
        buffer_out.data[j++]=SPI_READ;
        buffer_out.data[j++]=1;
        buffer_out.data[j++]=FLUSH;

        if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
        if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    }


    memset(buffer_out.data,0x00,buffer_out.len);
    memcpy(buffer_out.data+1,at_LV_EXIT,at_LV_EXIT_SIZE);

    if((op->send(op,&buffer_out)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}
    if((op->recv(op,&buffer_in)) == -1) {ret = -1; at_log(__FUNCTION__,"ret %ls",hid_error(op->dev_hnd));}


    return 0;
}
