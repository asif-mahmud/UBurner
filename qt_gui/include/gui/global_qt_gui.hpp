/**
* global_qt_gui.hpp
* GUI related global objects
*/
#ifndef OP_QT_GLOBAL_GUI_HPP
#define OP_QT_GLOBAL_GUI_HPP

#include <splash.hpp>
#include <main_win.hpp>
#include <opprog_int.hpp>
#include <QWidget>


extern Splash * splash;
extern MainWin * main_win;



void init_gui();
void move_to_center(QWidget * window);


#endif//OP_QT_GLOBAL_GUI_HPP
