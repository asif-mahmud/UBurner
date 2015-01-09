/**
* config_widget.hpp
* Author : Asif Mahmud Shimon
*/
#ifndef OP_GUI_CONFIG_WIDGET_HPP
#define OP_GUI_CONFIG_WIDGET_HPP

#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QTableWidget>
#include <opprog_int.hpp>
#include <QVBoxLayout>

#define CONFIG_BOX "Set Configurations and Options"
#define EEPROM_BOX "EEPROM Data Viewer"

#define PIC_CONFIG_NAME_SIZE 22
#define PIC_CONFIG_NAME_LEN 16
extern char PIC_CONFIG_NAMES[PIC_CONFIG_NAME_SIZE][PIC_CONFIG_NAME_LEN];

/**
* The layout of QDialog * configs will be changed according to the selected Chip
*/

class ConfigWidget : public QDialog
{
	Q_OBJECT 
	
	public :
		ConfigWidget(QWidget * parent = 0);
		virtual ~ConfigWidget();
		
	private :
        QGroupBox * avr_config;
        QCheckBox * avr_read_lock, * avr_read_lfuse, * avr_read_hfuse, * avr_read_efuse;
        QCheckBox * avr_write_lock, * avr_write_lfuse, * avr_write_hfuse, * avr_write_efuse;
        QLineEdit * avr_lock_edit, * avr_lfuse_edit,* avr_hfuse_edit,* avr_efuse_edit;
        QTableWidget * eeprom_viewer_table;

        QGroupBox * pic_config;
        QLineEdit * pic_config_viwer[22];

    public slots :
        void pgm_update_cb(const char *, const char *, int);
        void mcu_config_set_slot(int,int);
    private slots :
        void show_eeprom_slot(OP_MEM_TYPE);
        void show_config_slot(int,OP_MEM_TYPE);

        void set_avr_read_lock(bool);
        void set_avr_read_lfuse(bool);
        void set_avr_read_hfuse(bool);
        void set_avr_read_efuse(bool);
        void set_avr_write_lock(bool);
        void set_avr_write_lfuse(bool);
        void set_avr_write_hfuse(bool);
        void set_avr_write_efuse(bool);
        void update_avr_write_lock(QString);
        void update_avr_write_lfuse(QString);
        void update_avr_write_hfuse(QString);
        void update_avr_write_efuse(QString);
};

#endif //OP_GUI_CONFIG_WIDGET_HPP
