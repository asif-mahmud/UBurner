#include <main_win.hpp>
#include <gui_config.hpp>
#include <QtGui>

MainWin :: MainWin(QWidget * parent) : QMainWindow(parent)
{
	setMinimumWidth(640);
	setMinimumHeight(480);
	
	QDialog * dummy1 = new QDialog;
	QDialog * dummy2 = new QDialog;
	
	action_widget = new ActionWidget;
	config_widget = new ConfigWidget;
	about_mcu_widget = new AboutMCUWidget;
    about_widget = new AboutWidget;
    action_thread = new ActionThread;
	
	QTabWidget * tabs = new QTabWidget;
	tabs->addTab(action_widget,"Actions");
	tabs->addTab(config_widget,"EEPROM and Configuration");
	tabs->addTab(about_mcu_widget,"About MCU/Chip");
	tabs->addTab(about_widget,"Help and About");
	
	setCentralWidget(tabs);

	setWindowTitle(WINDOW_TITLE);
	setWindowIcon(QIcon(DEFAULT_WIN_ICON));
	
	connect(action_thread,SIGNAL(programmer_status_changed(bool,QString)),action_widget,SLOT(change_pgm_status_slot(bool,QString)));
    connect(action_widget,SIGNAL(update_about_mcu_signal(int,int)),about_mcu_widget,SLOT(update_mcu_viewer_slot(int,int)));
    connect(this,SIGNAL(pgm_update_signal(const char*,const char*,int)),action_widget,SLOT(pgm_update_slot(const char*,const char*,int)));
    connect(action_widget,SIGNAL(go_button_signal(bool,int,int)),action_thread,SLOT(perform_mem_actions(bool,int,int)));
    connect(this,SIGNAL(config_update_signal(const char*,const char*,int)),config_widget,SLOT(pgm_update_cb(const char*,const char*,int)));
    connect(action_widget,SIGNAL(update_about_mcu_signal(int,int)),config_widget,SLOT(mcu_config_set_slot(int,int)));
    connect(action_widget,SIGNAL(update_config_viewer_signal(int,OP_MEM_TYPE)),config_widget,SLOT(show_config_slot(int,OP_MEM_TYPE)));
    connect(action_widget,SIGNAL(erase_signal(int,int)),action_thread,SLOT(erase_slot(int,int)));

	action_thread->start_searching();
}

MainWin :: ~MainWin() {}


void MainWin :: Show() { this->show(); }

void MainWin:: pgm_update(const char *sender, const char *msg, int val)
{
    if(strcasecmp(msg,"show_eeprom_read") == 0)
    {
        emit config_update_signal(sender,msg,val);
    }else
        emit pgm_update_signal(sender,msg,val);
}
