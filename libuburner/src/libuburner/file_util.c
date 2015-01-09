#include <file_util.h>
#include <common.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <util.h>


#define MALLOC_FOR_FILE_LIST( X ) X=(file_list_t *)malloc(sizeof(file_list_t));\
				X->file = (file_info_t *)malloc(sizeof(file_info_t));\
				X->next = NULL 

file_list_t * get_file_list(const char * path)
{
	file_list_t * ret = NULL;
	char file_name[64];
	int r_check;

	DIR * dir = NULL;
	dir = opendir(path);
	if(dir != NULL)
	{
		struct dirent * dir_ent = NULL;
		struct stat d_stat;
		
		do
		{
			dir_ent = readdir(dir);
			if(dir_ent != NULL)
			{
				if((strcasecmp(dir_ent->d_name,".")==0) || 
					(strcasecmp(dir_ent->d_name,"..")==0))
						continue;
				else
				{
					sprintf(file_name,"%s"SEPERATOR"%s",path,dir_ent->d_name);
					
					r_check = stat(file_name,&d_stat);
					if(r_check != -1)
					{
						if(S_ISREG(d_stat.st_mode))
						{
							file_list_t * tmp;
							MALLOC_FOR_FILE_LIST(tmp);
							strcpy(tmp->file->name,file_name);
							strcpy(tmp->file->name_with_ext,dir_ent->d_name);
							strcpy(tmp->file->name_without_ext,dir_ent->d_name);
							char * last_dot = strrchr(tmp->file->name_without_ext,'.');
                            *last_dot = '\0';
							if(!ret)
							{
								ret = tmp;
							}else
							{							
								file_list_t * tmp2=ret;
								while(tmp2->next)
								{
									tmp2 = tmp2->next;
								}
								tmp2->next = tmp;
							}
						}
					}
				}
			}
		}while(dir_ent != NULL);
		
		closedir(dir);
	}

	return ret;
}

int save_hex_data(opprog_t * op, const char * file, mcu_info_t * mcu)
{
	int ret = -1;
	FILE * fd = fopen(file,"w");
	if(!fd) return ret;

	int i,sum=0,count=0,ext =0,s,base;
	char str[512],str1[512]="";
	
	/*** For PIC 10-16F Series ***/
	/** Rememberring that buffer is 8-bit wide ****/
	if((!strcasecmp(mcu->family,mcu_families[MICROCHIP_10F].name)) || 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_12F].name)) ||
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_16F].name)))
	{
		op_buffer_t * flash = op->memory->flash_read;
		
		if((flash->len%2)!=0) return ret;
		int x = 0xFFF,addr;
		if((!strcasecmp(mcu->family,mcu_families[MICROCHIP_16F].name)) ||
		(strstr(mcu->name,"12F6")!=NULL)||(strstr(mcu->name,"12f6")!=NULL)) x=0x3FFF;
		fprintf(fd,":020000040000FA\n");
		for(i=0;i<flash->len;i+=2){flash->data[i]&=x;flash->data[i+1]&=(x>>8);}
		for(i=0;i<flash->len;i+=2)//removinf leading 0xFFF
		{
			USHORT tmp = flash->data[i+1];
			tmp <<= 8;
			tmp |= flash->data[i];
			if(tmp<x) break;
		}
		int sizeW = flash->len /2;
		for(;i<flash->len;i+=2)
		{
			sum+=flash->data[i];
			sum+=flash->data[i+1];

			sprintf(str,"%02X%02X",flash->data[i],flash->data[i+1]);
			strcat(str1,str);
			count++;
			if(count==8 || ((i/2)==(sizeW-1)))
			{
				base = (i/2)-count+1;
				for(s=i;s>=base;s--)
				{
					USHORT tmp = flash->data[i+1];
					tmp >>= 8;
					tmp |= flash->data[i];
					if(tmp<x) break;
					sum-=flash->data[i];
					sum-=flash->data[i+1];
					str1[strlen(str1)-4]=0;
				}
				count-=(i/2)-s;
				addr=(s-count+1)*2;
				sum+=count*2+(addr&0xff)+(addr>>8);
				if(base>>15>ext)
				{
					ext=base>>15;
					fprintf(fd,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count) fprintf(fd,":%02X%04X00%s%02X\n",count*2,addr&0xFFFF,str1,(-sum)&0xff);
				str1[0]=0;
				count=0;
				sum=0;
			}
		}
		if((strstr(mcu->name,"16F1")!=NULL)||
			(strstr(mcu->name,"16f1")!=NULL) && 
			(op->memory->eeprom_read->len>0) )//As stated it's only for 16F1XXX
		{
			op_buffer_t * eeprom = op->memory->eeprom_read;

			if((eeprom->len%2)!=0) return ret;
			if(ext!=0x01) fprintf(fd,":020000040001F9\n");
			count = 0;
			sum = 0;
			for(i=0;i<eeprom->len;i++)
			{
				sum+=eeprom->data[i];	
				sprintf(str,"%02X00",eeprom->data[i]);
				strcat(str1,str);
				count++;
				if(count==8 || i==(eeprom->len-1))
				{
					for(s=i;s>i-count&&eeprom->data[s]>=0xFF;s--)
					{
						sum-=eeprom->data[s];
						str1[strlen(str1)-4]=0;
					}
					count -= i-s;
					addr = (s-count+1)*2+0xE000;
					sum += count*2+(addr&0xFF)+(addr>>8);
					if(count)
					{
						fprintf(fd,":%02X%04X00%s%02X\n",count*2,addr,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count =0 ;
					sum = 0;
				}
			}
		}
		fprintf(fd,":00000001FF\n");
		ret = 0;
	}
	/*** PIC 18F Series ********/
	else if(!strcasecmp(mcu->family,mcu_families[MICROCHIP_18F].name))
	{
		op_buffer_t * flash = op->memory->flash_read;
		fprintf(fd,":020000040000FA\n");
		for(i=0;(i<flash->len)&&(flash->data[i]==0xFF);i++);
		for(;i<flash->len;i++)
		{
			sum+=flash->data[i];
			sprintf(str,"%02X",flash->data[i]);
			strcat(str1,str);
			count++;
			if((count==16)||(i==(flash->len-1)))
			{
				base = i-count+1;
				for(s=i;s>=base&&flash->data[s]==0xFF;s--)
				{
					sum-=flash->data[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xFF)+((base>>8)&0xFF);
				if(base>>16>ext)
				{
					ext=base>>16;
					fprintf(fd,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count)
				{
					fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count=0;
				sum=0;
			}
		}
		//config_loaded
		op_buffer_t * config = op->memory->config_loaded;
		count=0;
		sum=0;
		for(i=0;i<8;i++)
		{
			sum+= config->data[i];
			sprintf(str,"%02X",config->data[i]);
			strcat(str1,str);
			count++;
			if(count==8)
			{
				fprintf(fd,":020000040020DA\n");
				base = i-count+1;
				for(s=i;(s>(i-count))&&(config->data[s]>=0xFF);s--)
				{
					sum-=config->data[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xFF)+((base>>8)&0xFF);
				if(count)
				{
					fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0] = 0;
				count =0 ;
				sum =0; 
			}
		}

		//config_read
		config = op->memory->config_read;
		count = 0;
		sum = 0;
		for(i=0;i<14;i++)
		{
			sum+= config->data[i];
			sprintf(str,"%02X",config->data[i]);
			strcat(str1,str);
			count++;
			if(count==14)
			{
				fprintf(fd,":020000040030CA\n");
				base=i-count+1;
				for(s=i;(s>(i-count))&&(config->data[s]>=0xFF);s--)
				{
					sum-=config->data[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(count)
				{
					fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count = 0;
				sum = 0;
			}
		}
		if(op->memory->eeprom_read->len > 0)
		{
			op_buffer_t * eeprom = op->memory->eeprom_read;
			fprintf(fd,":0200000400F00A\n");
			count = 0;
			sum = 0;
			for(i=0;i<eeprom->len;i++)
			{	
				sum+=eeprom->data[i];
				sprintf(str,"%02X",eeprom->data[i]);
				strcat(str1,str);
				count++;
				if(count == 16 || i==(eeprom->len-1))
				{
					base = i-count+1;
					for(s=i;(s>(i-count))&&(eeprom->data[s]>=0xFF);s--)
					{
						sum-=eeprom->data[s];
						str1[strlen(str1)-2]=0;
					}
					count-=i-s;
					sum+=count+(base&0xff)+((base>>8)&0xff);
					if(count)
					{
						fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count = 0;
					sum = 0;
				}
			}
		}
		fprintf(fd,":00000001FF\n");
		ret = 0;
	}
	/** PIC 24F-H-E & 30F & 33F-E **/
	else if((!strcasecmp(mcu->family,mcu_families[MICROCHIP_24F].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_24H].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_24E].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_30F].name))||
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_33F].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_33E].name)) )
	{
		op_buffer_t * flash = op->memory->flash_read;
		int valid, word;
		fprintf(fd,":020000040000FA\n");
		word = flash->data[0]+(flash->data[1]<<8)+(flash->data[2]<<16)+(flash->data[3]<<24);
		for(i=0;i<flash->len&&word==0xFFFFFFFF;i+=4)
			word = flash->data[i]+(flash->data[i+1]<<8)+(flash->data[i+2]<<16)+(flash->data[i+3]<<24);
		for(;i<flash->len;i++)
		{
			sum+=flash->data[i];
			sprintf(str,"%02X",flash->data[i]);
			strcat(str1,str);
			count++;
			if(count ==16 || i==flash->len-1)
			{
				base = i-count+1;
				valid = 0;
				for(s=base;s<=i&&!valid;s+=4)
				{
					if(flash->data[s]<0xFF || flash->data[s+1]<0xFF || flash->data[s+2] < 0xFF) 
						valid = 1;
				}
				sum += count+(base&0xff)+((base>>8)&0xff);
				if(base>>16>ext)
				{
					ext=base>>16;
					fprintf(fd,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count && valid)
				{
					fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count = 0;
				sum = 0;
			}
		}
		if(op->memory->config_read->len >0)
		{
			op_buffer_t * config = op->memory->config_read;
			fprintf(fd,":0200000401F009\n");
			sum = 0;
			count =0;
			for(i=0;i<config->len && i<48 ;i++)
			{
				sum+= config->data[i];
				sprintf(str,"%02X",config->data[i]);
				strcat(str1,str);
				count++;
				if(count == 4 || i==(config->len-1))
				{
					base=i-count+1;
					sum+=count+(base&0xff)+((base>>8)&0xff);
					if(count)
					{
						fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
					}
					str1[0]=0;
					count=0;
					sum=0;
				}
			}
		}
		if(op->memory->eeprom_read->len > 0)
		{
			int valid = 0;
			op_buffer_t * eeprom = op->memory->eeprom_read;
			fprintf(fd,":0200000400FFFB\n");
			str1[0] = 0;
			count = 0;
			sum = 0;
			for(i=0;i<eeprom->len;i+=2)
			{
				sum+=eeprom->data[i]+eeprom->data[i+1];
				sprintf(str,"%02X%02X0000",eeprom->data[i]&0xff,eeprom->data[i+1]&0xff);
				strcat(str1,str);
				count+=4;
				if(count == 16 || i==(eeprom->len-2))
				{
					base=2*i-count+4;
					for(s=base/2;s<=i&&!valid;s+=2)
					{
						if(eeprom->data[s]<0xFF || eeprom->data[s+1]<0xFF )
							valid = 1;
					}
					sum+=0xE0+count+(base&0xff)+(base>>8);
					if(count&&valid)
					{
						fprintf(fd,":%02X%04X00%s%02X\n",count,base+0xE000,str1,(-sum)&0xff);
					}
					str1[0] = 0;
					count = 0;
					sum = 0;
				}
			}
		}
		fprintf(fd,":00000001FF\n");
		ret = 0;
	}
	/** Atmel **/
	else if(!strcasecmp(mcu->family,mcu_families[ATMEL].name))
	{
		fprintf(fd,":020000040000FA\n");
		op_buffer_t * flash = op->memory->flash_read;
		for(i=0;i<flash->len&&flash->data[i]==0xFF;i++);
		for(;i<flash->len;i++)
		{
			sum+=flash->data[i];
			sprintf(str,"%02X",flash->data[i]);
			strcat(str1,str);
			count++;
			if(count==16||i==flash->len-1)
			{
				base=i-count+1;
				for(s=i;s>=base&&flash->data[s]==0xFF;s--)
				{
					sum-=flash->data[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(base>>16>ext)
				{
					ext=base>>16;
					fprintf(fd,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
				}
				if(count)
				{
					fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0] = 0;
				sum = 0;
				count = 0;
			}
		}
		count = 0;
		sum = 0;
		op_buffer_t * eeprom = op->memory->eeprom_read;
		for(i=0;i<eeprom->len;i++)
		{
			sum+=eeprom->data[i];
			sprintf(str,"%02X",eeprom->data[i]);
			strcat(str1,str);
			count++;
			if(count==16||i==eeprom->len-1)
			{
				base = i-count+1;
				for(s=i;s>i-count&&eeprom->data[s]>=0xFF;s--)
				{
					sum-=eeprom->data[s];
					str1[strlen(str1)-2]=0;
				}
				count-=i-s;
				sum+=count+(base&0xff)+((base>>8)&0xff);
				if(count)
				{
					fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
				}
				str1[0]=0;
				count = 0;
				sum = 0;
			}
		}
		fprintf(fd,":00000001FF\n");
		ret =0;
	}
	/** EEPROM **/
	else if(!strcasecmp(mcu->family,mcu_families[EEPROM].name))
	{
		if(strstr(file,".bin")||strstr(file,".BIN"))
		{
			#ifdef _WIN32
			//Maccioni is right. MS, you stop messing with bin files
			fd = freopen(file,"wb",fd);
			if(!fd)return ret;
			#endif
			fwrite(op->memory->eeprom_read->data,1,op->memory->eeprom_read->len,fd);
			ret = 0;
		}
		else
		{
			int valid;
			fprintf(fd,":020000040000FA\n");
			op_buffer_t * eeprom = op->memory->eeprom_read;
			for(i=0;i<eeprom->len;i++)
			{
				sum+=eeprom->data[i];
				sprintf(str,"%02X",eeprom->data[i]);
				strcat(str1,str);
				count++;
				if(count==16||i==eeprom->len-1)
				{
					valid = 0;
					for(s=0;str1[s]&&!valid;s++)
					{
						if(str1[s]!='F')valid=1;
					}
					if(valid)
					{
						base=i-count+1;
						sum+=count+(base&0xff)+((base>>8)&0xff);
						if(base>>16>ext)
						{
							ext=base>>16;
							fprintf(fd,":02000004%04X%02X\n",ext,(-6-ext)&0xff);
						}
						if(count)
						{
							fprintf(fd,":%02X%04X00%s%02X\n",count,base&0xFFFF,str1,(-sum)&0xff);
						}
					}
					str1[0] = 0;
					count = 0;
					sum = 0;
				}
			}
			fprintf(fd,":00000001FF\n");
			ret = 0;
		}
	}
	if(fd)fclose(fd);

	return ret;
}

int read_hex_data(opprog_t * op, const char *file, mcu_info_t * mcu)
{
	int ret =-1;
	int i, input_address = 0, ext_addr = 0, sum, valid, empty;
	int sizeCODE = 0,sizeEEPROM = 0;
	char s[256]="",line[256];
	
	FILE *f = fopen(file,"r");
	if(!f) return ret;

	/** PIC 10-12-16F **/
	if((!strcasecmp(mcu->family,mcu_families[MICROCHIP_10F].name)) || 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_12F].name)) ||
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_16F].name)))
	{
		UCHAR buffer[mcu->flash_memory_size],bufferEE[mcu->data_memory_size];
		int sizeM=0;
		memset(buffer,0xFF,sizeof(buffer));
		memset(bufferEE,0xFF,sizeof(bufferEE));
		for(;fgets(line,256,f);)
		{
			if(strlen(line)>9&&line[0]==':')
			{
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line)-11<hex_count*2)
				{
					return ret;//line too short
				}else
				{
					input_address=htoi(line+3,4);
					sum=0;
					for (i=1;i<=hex_count*2+9;i+=2) sum += htoi(line+i,2);
					if((sum&0xFF)!=0)
					{
						return ret;//checksum error
					}else
					{
						switch(htoi(line+7,2))
						{
							case 0 :
								if(ext_addr<=0x01&&input_address<0xE000)
								{
									sizeM=(ext_addr<<16)+input_address+hex_count;
									if(sizeM>sizeCODE)sizeCODE=sizeM;
									for (i=0;i<hex_count;i++)
									{
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}else if(ext_addr==0x1&&input_address>=0xE000&&input_address<0xF000)
								{
									sizeM=(input_address-0xE000+hex_count)/2;
									if(sizeM>sizeEEPROM)sizeEEPROM=sizeM;
									for(i=0;i<hex_count;i+=2)
									{
										bufferEE[(input_address-0xE000)/2+i/2]=htoi(line+9+i*2,2);
									}
								}
								break;
							case 4 :
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default :
								break;
						}
					}
				}
			}
		}
        op->clear_memory(op,FLASH_LOADED);
        op->clear_memory(op,EEPROM_LOADED);
        if(sizeCODE)op->append_data(op,FLASH_LOADED,buffer,sizeCODE);
        if(sizeEEPROM)op->append_data(op,EEPROM_LOADED,bufferEE,sizeEEPROM);
		ret = 0;
	}
	/** PIC 18F **/
	else if(!strcasecmp(mcu->family,mcu_families[MICROCHIP_18F].name))
	{
		UCHAR buffer[mcu->flash_memory_size],bufferEE[mcu->data_memory_size],memID[8],memCONFIG[14];
		int sizeM,sizeMEMID=0,sizeCONFIG=0;
		memset(buffer,0xFF,sizeof(buffer));
		memset(bufferEE,0xFF,sizeof(bufferEE));
		memset(memID,0xFF,sizeof(memID));
		memset(memCONFIG,0xFF,sizeof(memCONFIG));
		for(;fgets(line,256,f);)
		{
			if(strlen(line)>9&&line[0]==':')
			{
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2)
					return ret; //line too short
				else
				{
					input_address=htoi(line+3,4);
					sum=0;
					for (i=1;i<=hex_count*2+9;i+=2)
						sum += htoi(line+i,2);
					if ((sum & 0xff)!=0) return ret; //check sum error
					else
					{
						switch(htoi(line+7,2))
						{
							case 0 :
								if(ext_addr<0x20)
								{
									sizeM=(ext_addr<<16)+input_address+hex_count;
									if(sizeM>sizeCODE)sizeCODE=sizeM;
									for(i=0;i<hex_count;i++)
									{
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}else if(ext_addr==0x20&&input_address<8)
								{
									for (i=0;i<hex_count;i++)
									{
										memID[input_address+i]=htoi(line+9+i*2,2);
									}
									sizeMEMID = 8;
								}else if(ext_addr==0x30&&input_address<14)
								{
									for (i=0;i<hex_count;i++)
									{
										memCONFIG[input_address+i]=htoi(line+9+i*2,2);
									}
									sizeCONFIG = 14;
								}else if(ext_addr==0xF0&&input_address<0x1000)
								{
									for (i=0;i<hex_count;i++)
									{
										bufferEE[input_address+i]=htoi(line+9+i*2,2);
									}
									sizeM=input_address+hex_count;
									if(sizeM>sizeEEPROM)sizeEEPROM=sizeM;
								}
								break;
							case 4 :
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default :
								break;
						}
					}
				}
			}
		}
        op->clear_memory(op,FLASH_LOADED);
        op->clear_memory(op,EEPROM_LOADED);
        op->clear_memory(op,CONFIG_LOADED);
        if(sizeCODE)op->append_data(op,FLASH_LOADED,buffer,sizeCODE);
        if(sizeEEPROM)op->append_data(op,EEPROM_LOADED,bufferEE,sizeEEPROM);
        if(sizeMEMID)op->append_data(op,CONFIG_LOADED,memID,sizeMEMID);
        if(sizeCONFIG)op->append_data(op,CONFIG_LOADED,memCONFIG,sizeCONFIG);
		ret = 0;
	}
	/** PIC 24F-H-E & 30F & 33F-E **/
	else if((!strcasecmp(mcu->family,mcu_families[MICROCHIP_24F].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_24H].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_24E].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_30F].name))||
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_33F].name))|| 
		(!strcasecmp(mcu->family,mcu_families[MICROCHIP_33E].name)) )
	{
		UCHAR buffer[mcu->flash_memory_size],bufferEE[mcu->data_memory_size],memCONFIG[48],memUSERID[8];
		int sizeCONFIG=0,sizeUSERID=0;
		memset(buffer,0xFF,sizeof(buffer));
		memset(bufferEE,0xFF,sizeof(bufferEE));
		memset(memCONFIG,0xFF,sizeof(memCONFIG));
		memset(memUSERID,0xFF,sizeof(memUSERID));
		for(;fgets(line,256,f);)
		{
			if(strlen(line)>9&&line[0]==':')
			{
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2)
					return ret;//line too short
				else
				{
					input_address=htoi(line+3,4);
					sum = 0;
					for (i=1;i<=hex_count*2+9;i+=2)
						sum += htoi(line+i,2);
					if((sum & 0xff)!=0)
						return ret;//checksum error
					else
					{
						switch(htoi(line+7,2))
						{
							case 0:
								if(ext_addr<0x20)
								{
									int end1=(ext_addr<<16)+input_address+hex_count;
									if(sizeCODE<end1)sizeCODE=end1;
									for (i=0;i<hex_count;i++)
									{
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}else if(ext_addr==0x1F0&&input_address<48)
								{
									sizeCONFIG=input_address+hex_count;
									for (i=0;i<hex_count;i++)
									{
										memCONFIG[input_address+i]=htoi(line+9+i*2,2);
									}
								}else if(ext_addr==0xFF&&input_address>=0xE000)
								{
									for (i=0;i<hex_count;i++)
									{
										bufferEE[input_address-0xE000+i]=htoi(line+9+i*2,2);
									}
									sizeEEPROM=input_address-0xE000+hex_count;
								}else if(ext_addr==0x100&&input_address<8)
								{
									sizeUSERID=input_address+hex_count;
									for (i=0;i<hex_count&&(i+input_address)<8;i++)
									{
										memUSERID[input_address+i]=htoi(line+9+i*2,2);
									}
								}
								break;
							case 4 :
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default :
								break;
						}
					}
				}
			}
		}
        op->clear_memory(op,FLASH_LOADED);
        op->clear_memory(op,EEPROM_LOADED);
        op->clear_memory(op,CONFIG_LOADED);
        if(sizeCODE)op->append_data(op,FLASH_LOADED,buffer,sizeCODE);
        if(sizeEEPROM)op->append_data(op,EEPROM_LOADED,bufferEE,sizeEEPROM);
        if(sizeUSERID)op->append_data(op,CONFIG_LOADED,memUSERID,sizeUSERID);
        if(sizeCONFIG)op->append_data(op,CONFIG_LOADED,memCONFIG,sizeCONFIG);

		ret = 0;
	}
	/** Atmel **/
	else if(!strcasecmp(mcu->family,mcu_families[ATMEL].name))
    {
		UCHAR buffer[mcu->flash_memory_size],bufferEE[mcu->data_memory_size];
		memset(buffer,0xFF,sizeof(buffer));
		memset(bufferEE,0xFF,sizeof(bufferEE));
		for(;fgets(line,256,f);)
		{
			if(strlen(line)>9&&line[0]==':')
			{
				int hex_count = htoi(line+1, 2);
				if((int)strlen(line) - 11 < hex_count * 2)
					return ret;//line too short
				else
				{
					input_address=htoi(line+3,4);
					sum = 0;
					for (i=1;i<=hex_count*2+9;i+=2)
						sum += htoi(line+i,2);
					if ((sum & 0xff)!=0)
						return ret;//checksum error
					else
					{
						switch(htoi(line+7,2))
						{
							case 0 :
								if(ext_addr<0x20)
								{
									sizeCODE=input_address+hex_count;
									for (i=0;i<hex_count;i++)
									{
										buffer[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
								}else if(ext_addr==0&&input_address<0x1000)
								{
									for (i=0;i<hex_count;i++)
									{
										bufferEE[input_address+i]=htoi(line+9+i*2,2);
									}
									sizeEEPROM = input_address+hex_count;
								}
								break;
							case 4 :
								if(strlen(line)>14)	ext_addr=htoi(line+9,4);
								break;
							default :
								break;
						}
					}
				}
			}
		}
        op->clear_memory(op,FLASH_LOADED);
        op->clear_memory(op,EEPROM_LOADED);
        if(sizeCODE)op->append_data(op,FLASH_LOADED,buffer,sizeCODE);
        if(sizeEEPROM)op->append_data(op,EEPROM_LOADED,bufferEE,sizeEEPROM);
		ret = 0;
	}
	/** EEPROM **/
	else if(!strcasecmp(mcu->family,mcu_families[EEPROM].name))
	{
		if(strstr(file,".bin")||strstr(file,".BIN"))
		{
			//again that win32  shit
			#ifdef _WIN32
			f=freopen(file,"rb",f);
			if(!f) return ret;
			#endif
			fseek(f, 0L, SEEK_END);
			int sizeEEPROM=ftell(f);
			fseek(f, 0L, SEEK_SET);
			if(sizeEEPROM>0x1000000) sizeEEPROM=0x1000000;//max 16Mbytes
			UCHAR bufferEE[sizeEEPROM];
			fread(bufferEE,1,sizeEEPROM,f);
			op->fill_up_memory(op,EEPROM_LOADED,bufferEE,sizeEEPROM);
		}else
		{
			UCHAR bufferEE[mcu->data_memory_size];
			int sizeEEPROM =0;
			memset(bufferEE,0xFF,sizeof(bufferEE));
			for(;fgets(line,256,f);)
			{
				if(strlen(line)>9&&line[0]==':')
				{
					int hex_count = htoi(line+1, 2);
					if((int)strlen(line) - 11 < hex_count * 2)
						return ret;//line too short
					else
					{
						input_address=htoi(line+3,4);
						sum = 0;
						int end1;
						for (i=1;i<=hex_count*2+9;i+=2) sum+=htoi(line+i,2);
						if ((sum & 0xff)!=0)
						{
							return ret;//checksum error
						}else
						{
							switch(htoi(line+7,2))
							{
								case 0:
									end1=(ext_addr<<16)+input_address+hex_count;
									if(end1>=0x1000000) break; //max 16MB
									if(sizeEEPROM<end1) sizeEEPROM = end1;
									for (i=0;i<hex_count;i++)
									{
										bufferEE[(ext_addr<<16)+input_address+i]=htoi(line+9+i*2,2);
									}
									break;
								case 4 :
									if(strlen(line)>14)	ext_addr=htoi(line+9,4);
									break;
								default :
									break;
							}
						}
					}
				}
            }
            op->clear_memory(op,EEPROM_LOADED);
            if(sizeEEPROM)op->append_data(op,EEPROM_LOADED,bufferEE,sizeEEPROM);
			ret = 0;
		}
	}
	fclose(f);
	return ret;
}
