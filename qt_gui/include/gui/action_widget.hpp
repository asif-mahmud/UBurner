/**
 * action_widget.hpp
 * Author : Asif Mahmud Shimon
 */
#ifndef OP_GUI_ACTION_WIDGET_HPP
#define OP_GUI_ACTION_WIDGET_HPP

#include <QtGui>
#include <opprog_int.hpp>

#define MCU_SEL_GRP		"Select MCU by Family"
#define ACTION_GRP		"Select Actions"
#define PGM_STAT_GRP	"Programmer Status"
#define MSG_GRP			"Message Box"
#define FLASH_GRP		"Flash Code Viewer"
#define MCU_FAMILY_LABEL "MCU Family : "
#define MCU_NAME_LABEL  "MCU/Chip : "

class ActionWidget : public QDialog
{
	Q_OBJECT
	
	public :
		ActionWidget(QWidget * parent = 0);
		virtual ~ActionWidget();
		
	private :
		QComboBox * mcu_family_combo, * mcu_name_combo;
		QLabel * mcu_family_label, * mcu_name_label;
		QCheckBox 		*read_first,
						*read_flash_chkbox,
						*read_eeprom_chkbox,
						*read_config_chkbox,
						*write_flash_chkbox,
						*write_config_chkbox,
						*write_eeprom_chkbox;
		QPushButton		* open_flash_button, * open_eeprom_button, * go_button, * erase_button;
		QLabel * pgm_status_label, *pgm_status_icon_label, * pgm_info_label;
		QProgressBar * progress_bar;
		QTextBrowser * msg_browser;
        QTableWidget   * flash_viewer_table;

        void map_mcu_list();
        void setup_operations();

	public slots :
		void change_pgm_status_slot(bool,QString);
        void pgm_update_slot(const char *, const char*,int);
    private slots :
        void change_mcu_list_slot(int);
        void change_mcu_info_slot(int);
        void open_flash_slot();
        void update_msg_browser_slot(QString);
        void go_button_slot();
        void show_flash_slot(OP_MEM_TYPE);
        void erase_signal_slot();

    signals:
        void update_about_mcu_signal(int,int);
        void update_config_viewer_signal(int, OP_MEM_TYPE);
        void update_msg_browser_signal(QString);
        void go_button_signal(bool,int,int);
        void show_flash_signal(OP_MEM_TYPE);
        void erase_signal(int,int);
};

#endif //OP_GUI_ACTION_WIDGET_HPP
