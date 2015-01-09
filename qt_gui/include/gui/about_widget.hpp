/**
 * about_widget.hpp
 * Author : Asif Mahmud Shimon
 */
#ifndef OP_GUI_ABOUT_WIDGET_HPP
#define OP_GUI_ABOUT_WIDGET_HPP

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>

class AboutWidget : public QDialog
{
	Q_OBJECT
	
	public :
		AboutWidget(QWidget * parent = 0);
		virtual ~AboutWidget();
		
	private :
		QPushButton * license_button, * help_button, * about_button;
		QTextBrowser * file_viewer;
};

#endif //OP_GUI_ABOUT_WIDGET_HPP
