/**
* global_qt_gui.cpp
* Author : Asif Mahmud Shimon
*/
#include <global_qt_gui.hpp>
#include <gui_config.hpp>
#include <QApplication>
#include <QString>
#include <QFile>
#include <QDesktopWidget>

Splash * splash;
MainWin * main_win;

void map_mcu_list();

void init_gui()
{
    register_mcu_list_update_cb(loader_update_cb);
    register_plugin_load_update_cb(loader_update_cb);
    opprog = op_init();

	splash = new Splash();
	main_win = new MainWin();
	
	QFile file(DEFAULT_STYLE);
	file.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(file.readAll());
	qApp->setStyleSheet(StyleSheet);

	splash->view_splash();

	splash->show_msg("Callbacks registered");
	Sleep(500);


	if(opprog)
	{
		splash->show_msg("libopprog initialised");
		Sleep(500);
	
		int total = 0;
        mcus = load_mcu_list(DEFAULT_MCUS_DIR,&total);
        map_mcu_list();
        splash->show_msg("Total "+ QString::number(total)+" MCUs found");
		Sleep(1000);
		
		/**
		 * FIXME: Find a generic WAY to load programmers
		 */
        pgms = load_programmers(&total);
        programmer_list_t * tmp = pgms;
        for(;tmp;tmp=tmp->next)
            tmp->pgm->register_update_cb(pgm_update_cb);
		splash->show_msg("Total "+QString::number(total)+" programmers found ");
		Sleep(1000);

        plugins = load_plugins(DEFAULT_PLUGINS_DIR,&total);
		splash->show_msg("Total "+QString::number(total)+" plugins found ");
		Sleep(1000);

		QApplication::processEvents();		
		main_win->Show();
		move_to_center(main_win);
		splash->finish(main_win);
	}
}

void move_to_center(QWidget * window)
{
	QDesktopWidget * desktop = QApplication::desktop();
	window->move((desktop->width()/2)-(window->width()/2),
			(desktop->height()/2)-(window->height()/2));
}

void map_mcu_list()
{
    for(;mcus;mcus=mcus->next)
    {
        for(int i=0;i<AVAILABLE_MCU_FAMILY_NUM;i++)
        {
            if(strcasecmp(mcus->mcu->family,mcu_families[i].name) == 0)
            {
                mapped_mcus[i].push_back(mcus->mcu);
                break;
            }
        }
    }
}
