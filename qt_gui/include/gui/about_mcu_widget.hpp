/**
 * about_mcu_widget.hpp
 * Author : Asif Mahmud Shimon
 */
#ifndef OP_GUI_ABOUT_MCU_WIDGET_HPP
#define OP_GUI_ABOUT_MCU_WIDGET_HPP


#include <QtGui>
#include <mcu_viewer.hpp>

class AboutMCUWidget : public QDialog 
{
	Q_OBJECT	
	public :
		AboutMCUWidget(QWidget * parent = 0);
		virtual ~AboutMCUWidget();
		
    private :
        QTextBrowser * info_box;
        McuViewer 	* mcu_viewer;

    public slots:
        void update_mcu_viewer_slot(int,int);

    signals:
        void update_mcu_viewer_signal(int,int);
};

#endif //OP_GUI_ABOUT_MCU_WIDGET_HPP
