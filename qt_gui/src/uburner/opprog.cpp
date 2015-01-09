#include <global_qt_gui.hpp>
#include <QApplication>
#include <QString>
#include <opprog_int.hpp>

opprog_t 		* opprog;
mcu_list_t 		* mcus;
programmer_list_t 	* pgms;
plugin_list_t 		* plugins;
uint8_t last_flash_action = 0;
uint8_t last_eeprom_action = 0;
uint8_t last_config_action = 0;
map<int , vector<mcu_info_t *> > mapped_mcus;
op_mem_options_t  memory_options = { NULL , 0 };

_AVR_read_config_operations_t avr_read_config_operations = {
    0,0,0,0,0,0,0,0
};

void loader_update_cb(const char * sender, const char * msg, int d_val)
{
	QString str(sender);
	str += ":";
	str += msg;
	splash->show_msg(str);
	QApplication::processEvents();
}

void pgm_update_cb(const char *sender, const char *msg, int val)
{
    main_win->pgm_update(sender,msg,val);
}
