/**
* main_win.hpp
* Author : Asif Mahmud Shimon
*/
#ifndef MAIN_WIN_HPP
#define MAIN_WIN_HPP

#include <QMainWindow>
#include <action_widget.hpp>
#include <config_widget.hpp>
#include <about_mcu_widget.hpp>
#include <about_widget.hpp>
#include <action_thread.hpp>

class MainWin : public QMainWindow
{
	Q_OBJECT

	public :
		MainWin(QWidget * parent = NULL);
		virtual ~MainWin();
		void Show();
        void pgm_update(const char *, const char*,int);
		
	private : 
		ActionWidget * action_widget;
        ConfigWidget * config_widget;
        AboutWidget	* about_widget;
        ActionThread * action_thread;
        AboutMCUWidget * about_mcu_widget;

    signals:
        void pgm_update_signal(const char *, const char *, int);
        void config_update_signal(const char *, const char *, int);
};

#endif//MAIN_WIN_HPP
