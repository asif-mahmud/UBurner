#include <about_mcu_widget.hpp>
#include <opprog_int.hpp>
#include <vector>

using namespace std;

AboutMCUWidget :: AboutMCUWidget(QWidget * parent ): QDialog(parent)
{
    QGroupBox * about_box = new QGroupBox("MCU/Chip Info : ");
    QVBoxLayout * info_l = new QVBoxLayout;
    info_box = new QTextBrowser;
    info_box->setMinimumWidth(300);
    info_l->addWidget(info_box);
    about_box->setLayout(info_l);
	 
    QGroupBox * mcu_view_box = new QGroupBox("MCU/Chip Viewer");
    mcu_viewer = new McuViewer;
    QVBoxLayout * mcu_view_lt = new QVBoxLayout;
    mcu_view_lt->addWidget(mcu_viewer);
    mcu_view_box->setLayout(mcu_view_lt);
	 
    QHBoxLayout * main_lt = new QHBoxLayout;
    main_lt->addWidget(about_box);
    main_lt->addWidget(mcu_view_box);
	 
    setLayout(main_lt);
    connect(this,SIGNAL(update_mcu_viewer_signal(int,int)),mcu_viewer,SLOT(update_mcu_drawer_slot(int,int)));
}

AboutMCUWidget ::~AboutMCUWidget() {}

void AboutMCUWidget :: update_mcu_viewer_slot(int family, int mcu)
{
    info_box->clear();
    mcu_info_t * info = mapped_mcus[family].at(mcu);
    QString info_str;
    info_str += QString("Name : ")+info->name+QString("\n");
    info_str += QString("Family : ")+info->family+QString("\n");
    info_str += QString("ID : ")+info->id+QString("\n");
    info_str += QString("Code Memory Size : ")+QString::number(info->flash_memory_size)+QString("\n");
    info_str += QString("Data Memory Size : ")+QString::number(info->data_memory_size)+QString("\n");
    info_str += QString("RAM/SRAM size : ")+QString::number(info->ram_size)+QString("\n");
    info_str += QString("Operating \nVoltage Range : ")+info->operating_voltage_range+QString("\n");
    info_str += QString("Programming\nVoltage Range : ")+info->pgm_voltage_range+QString("\n");
    info_str += QString("Programming\nPin Map : \n");
    QString map(info->pgm_pin_map);
    qDebug()<<map;
    QRegExp expr("[:;]");
    vector<int> pin_numbers;
    vector<QString> pin_names;
    QStringList pin_list = map.split(expr,QString::SkipEmptyParts);
    for(int i=0;(i+1)<pin_list.size();i+=2)
    {
        pin_numbers.push_back(pin_list.at(i).toInt());
        pin_names.push_back(QString(pin_list.at(i+1)));
    }

    for(int i =0; i<pin_names.size();i++)
    {
        info_str += QString("\t")+pin_names[i]+QString(" --> ")+QString::number(pin_numbers[i])+QString("\n");
    }

    info_str += QString("Speed Grade : ")+info->speed_grade+QString("\n");

    info_box->append(info_str);
    emit update_mcu_viewer_signal(family,mcu);
}
