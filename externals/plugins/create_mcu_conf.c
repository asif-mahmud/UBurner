/**
* create_mcu_conf.c
* A plugin to write mcu_list_t in xml file
* Author : Asif Mahmud Shimon
*/
#include "create_mcu_conf.h"

void create_mcu_conf_init( plugin_t * plugin)
{
	if(plugin)
	{
		sprintf(plugin->name,"create_mcu_conf");
		sprintf(plugin->type,"cmdtools");
		sprintf(plugin->screen_name,"Create MCU Configuration");

		plugin->run = create_mcu_conf_run;
	}
}

void create_mcu_conf_run(opprog_t * op, mcu_info_t * mcu,char * argv, int d_val)
{
	//argv == name of the config file
	//mcu == an mcu_config_t pointer
	//d_val == optional

	char * path = argv;
	int ret;

	FILE * fd = fopen(path,"w");
	if(fd)
	{
		fprintf(fd,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<mcu_list>\n");
		
		char str[1024];
		sprintf(str,"<mcu>\n\
				\t<id>%s</id>\n\
				\t<name>%s</name>\n\
				\t<family>%s</family>\n\
				\t<flash_memory_size>%d</flash_memory_size>\n\
				\t<data_memory_size>%d</data_memory_size>\n\
				\t<ram_size>%d</ram_size>\n\
				\t<operatine_voltage_range>%s</operatine_voltage_range>\n\
				\t<pgm_voltage_range>%s</pgm_voltage_range>\n\
				\t<pgm_pin_map>%s</pgm_pin_map>\n\
				\t<speed_grade>%s</speed_grade>\n\
				\t<package_type>%d</package_type>\n\
				\t<pin_count>%d</pin_count>\n</mcu>\n",
				mcu->id,
				mcu->name,
				mcu->family,
				mcu->flash_memory_size,
				mcu->data_memory_size,
				mcu->ram_size,
				mcu->operating_voltage_range,
				mcu->pgm_voltage_range,
				mcu->pgm_pin_map,
				mcu->speed_grade,
				mcu->package.package,
				mcu->package.pin_count);
		ret = (int)fwrite(str,1,strlen(str),fd);
		
		fprintf(fd,"</mcu_list>\n");
		fclose(fd);
	}
}
