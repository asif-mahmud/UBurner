#include <util.h>
#include <instructions.h>
#include <programmer.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

int strtrim(const char * src,char * dest)
{
	if(!src)return 0;
	int len = strlen(src);	
	int i = 0;
	while(isspace(src[i]))i++;
	while(isspace(src[len-1]))len--;
	len -= i;
	if(len<=0)return 0;
	if(!dest)dest=(char *)malloc(len+1);
	strncpy(dest,src+i,len);
	dest[len]=0;
	return len;
}

int htoi(const char * hex, int len)
{
	int i ;
	unsigned int ret = 0;

	for(i=0;i<len;i++)
	{
		ret<<= 4;
		if (hex[i] >= '0' && hex[i] <= '9') ret += hex[i] - '0';
		else if (hex[i] >= 'a' && hex[i] <= 'f') ret += hex[i] - 'a' + 10;
		else if (hex[i] >= 'A' && hex[i] <= 'F') ret += hex[i] - 'A' + 10;
	}
	return ret;
}

#ifdef _WIN32
char *strtok_r(char *str, const char *delim, char **save)
{
    char *res, *last;

    if( !save )
        return strtok(str, delim);
    if( !str && !(str = *save) )
        return NULL;
    last = str + strlen(str);
    if( (*save = res = strtok(str, delim)) )
    {
        *save += strlen(res);
        if( *save < last )
            (*save)++;
        else
            *save = NULL;
    }
    return res;
}

char * strcasestr(const char * s, const char * find)
{
	char c, sc;
	size_t len;
	if ((c = *find++) != 0) {
		c = tolower((unsigned char)c);
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return (NULL);
			} while ((char)tolower((unsigned char)sc) != c);
		} while (strncasecmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}

#endif

ULONG get_tick_count()
{
	struct timeb now;
	ftime(&now);
	return now.time*1000+now.millitm;
}

op_hardware_t * get_hardware_id(opprog_t * op)
{
	op_hardware_t * ret = (op_hardware_t *)malloc(sizeof(op_hardware_t));
	op_buffer_t bufferU,bufferI;
	bufferU.len = OP_PGM_BUFFER_SIZE;
	bufferU.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
	bufferI.len = OP_PGM_BUFFER_SIZE;
	bufferI.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
	int j = 1;
	bufferU.data[j++]=PROG_RST;
	bufferU.data[j++]=FLUSH;
	
	if((op->send(op,&bufferU))==-1) { return NULL;}
	if((op->recv(op,&bufferI))==-1) { return NULL;}
	
	ret->firmware_version = (bufferI.data[2]<<16)+(bufferI.data[3]<<8)+bufferI.data[4];
	ret->hardware_id = bufferI.data[7];
	if(ret->hardware_id == 1) sprintf(ret->pgm_mcu,"18F2550");
	else if(ret->hardware_id == 2)sprintf(ret->pgm_mcu,"18F2450");
	else if(ret->hardware_id == 3)sprintf(ret->pgm_mcu,"18F2458/2553");
	
	return ret;
}

double start_HV_regulator(opprog_t * op ,double  V)
{
	int j=1,z;
	int ret = 0;
	int vreg=(int)(V*10.0);
	op_buffer_t bufferU,bufferI;
	bufferU.len = OP_PGM_BUFFER_SIZE;
	bufferU.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
	bufferI.len = OP_PGM_BUFFER_SIZE;
	bufferI.data = (UCHAR *)calloc(OP_PGM_BUFFER_SIZE,sizeof(UCHAR));
	ULONG t0,t;
	if(V==-1)
	{
		bufferU.data[j++]=VREG_DIS;			//disable HV regulator
		bufferU.data[j++]=FLUSH;
		if((op->send(op,&bufferU))==-1) { ret = -1;return ret;}
		Sleep(50);
		if((op->recv(op,&bufferI))==-1) { ret = -1;return ret;}
		return -1;
	}
	op_hardware_t * hw = get_hardware_id(op);
	t0 = get_tick_count();
	t = t0;
	bufferU.data[j++]=VREG_EN;			//enable HV regulator
	bufferU.data[j++]=SET_VPP;
	bufferU.data[j++]=vreg;
	bufferU.data[j++]=SET_PARAMETER;
	bufferU.data[j++]=SET_T3;
	bufferU.data[j++]=2000>>8;
	bufferU.data[j++]=2000&0xff;
	bufferU.data[j++]=WAIT_T3;
	bufferU.data[j++]=READ_ADC;
	bufferU.data[j++]=FLUSH;
	if((op->send(op,&bufferU))==-1) { ret = -1;return ret;}
	Sleep(20);
	if((op->recv(op,&bufferI))==-1) { ret = -1;return ret;}
	for(z=1;z<OP_PGM_BUFFER_SIZE-2&&bufferI.data[z]!=READ_ADC;z++);
	int v=(bufferI.data[z+1]<<8)+bufferI.data[z+2];
	if(v=0) return -1;
	memset(bufferU.data,0x00,sizeof(bufferU.data));
	j=1;
	bufferU.data[j++]=WAIT_T3;
	bufferU.data[j++]=READ_ADC;
	bufferU.data[j++]=FLUSH;
	for(;(v<(vreg/10.0-1)*V_GAIN||v>(vreg/10.0+1)*V_GAIN)&&t<t0+1500;t=get_tick_count())
	{
		if((op->send(op,&bufferU))==-1) { ret = -1;return ret;}
		Sleep(20);
		if((op->recv(op,&bufferI))==-1) { ret = -1;return ret;}
		for(z=1;z<OP_PGM_BUFFER_SIZE-2&&bufferI.data[z]!=READ_ADC;z++);
		v=(bufferI.data[z+1]<<8)+bufferI.data[z+2];
		if(hw->hardware_id==3) v>>=2;		//if 12 bit ADC
	}
	if(hw)free(hw);
	return ((double)v)/V_GAIN;
}
