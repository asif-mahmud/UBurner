#include <action_widget.hpp>
#include <gui_config.hpp>
#include <global_qt_gui.hpp>

ActionWidget :: ActionWidget(QWidget * parent) : QDialog(parent)
{
	QGroupBox * mcu_grp, * actions_grp, * pgm_stat_grp, * msg_grp, * flash_grp;
	mcu_grp = new QGroupBox(MCU_SEL_GRP);
	actions_grp = new QGroupBox(ACTION_GRP);
	pgm_stat_grp = new QGroupBox(PGM_STAT_GRP);
	msg_grp = new QGroupBox(MSG_GRP);
	flash_grp = new QGroupBox(FLASH_GRP);

	QVBoxLayout * mcu_lt = new QVBoxLayout;
	mcu_family_combo = new QComboBox;
    mcu_family_label = new QLabel(MCU_FAMILY_LABEL);
    for(int i=0;i<AVAILABLE_MCU_FAMILY_NUM;i++)
        mcu_family_combo->addItem(mcu_families[i].name);
	mcu_name_combo = new QComboBox;
    mcu_name_label = new QLabel(MCU_NAME_LABEL);
	mcu_lt->addWidget(mcu_family_combo);
	mcu_lt->addWidget(mcu_family_label);
	mcu_lt->addWidget(mcu_name_combo);
	mcu_lt->addWidget(mcu_name_label);
	mcu_grp->setLayout(mcu_lt);
	
	QVBoxLayout * action_lt = new QVBoxLayout;
	read_first = new QCheckBox("Read First");
	read_first->setChecked(true);
	read_flash_chkbox = new QCheckBox("Read Flash Code");
	read_eeprom_chkbox = new QCheckBox("Read EEPROM");
	read_config_chkbox = new QCheckBox("Read Configurations");
	write_flash_chkbox = new QCheckBox("Write Flash Code");
	write_eeprom_chkbox = new QCheckBox("Write EEPROM");
	write_config_chkbox = new QCheckBox("Write Configurations");
	go_button = new QPushButton("Go");
	go_button->setIcon(QIcon(DEFAULT_GO_ICON));
	erase_button = new QPushButton("Erase");
	erase_button->setIcon(QIcon(DEFAULT_ERASE_ICON));
	open_flash_button = new QPushButton("Open Flash Data");
	open_eeprom_button = new QPushButton("Open EEPROM Data");
	action_lt->addWidget(read_first);
	action_lt->addWidget(read_flash_chkbox);
	action_lt->addWidget(read_eeprom_chkbox);
	action_lt->addWidget(read_config_chkbox);
	action_lt->addWidget(write_flash_chkbox);
	action_lt->addWidget(write_eeprom_chkbox);
	action_lt->addWidget(write_config_chkbox);
	action_lt->addWidget(open_flash_button);
	action_lt->addWidget(open_eeprom_button);
	action_lt->addWidget(go_button);
	action_lt->addWidget(erase_button);
	actions_grp->setLayout(action_lt);
	
	QVBoxLayout * pgm_stat_lt = new QVBoxLayout;
	QHBoxLayout * pgm_stat_up = new QHBoxLayout;
    pgm_status_label = new QLabel("Programmer Not Found");
	pgm_status_icon_label = new QLabel;
    pgm_status_icon_label->setPixmap(QPixmap(RED_ICON));
	pgm_info_label = new QLabel("");
	progress_bar = new QProgressBar;
    progress_bar->setVisible(false);
	pgm_stat_up->addWidget(pgm_status_label);
	pgm_stat_up->addWidget(pgm_status_icon_label);
	pgm_stat_lt->addLayout(pgm_stat_up);
	pgm_stat_lt->addWidget(pgm_info_label);
	pgm_stat_lt->addWidget(progress_bar);
	pgm_stat_grp->setLayout(pgm_stat_lt);
	
	QVBoxLayout * msg_lt = new QVBoxLayout;
	msg_browser = new QTextBrowser;
	msg_browser->setReadOnly(true);
	msg_lt->addWidget(msg_browser);
	msg_grp->setLayout(msg_lt);
	
	QVBoxLayout * flash_lt = new QVBoxLayout;
    flash_viewer_table = new QTableWidget(0,128);
    flash_viewer_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    flash_viewer_table->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    flash_viewer_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList h_headers;
    for(int i=0;i<128;i++)
        h_headers.push_back(QString::number(i,16));
    flash_viewer_table->setHorizontalHeaderLabels(h_headers);
	flash_lt->addWidget(flash_viewer_table);
	flash_grp->setLayout(flash_lt);
	
	QVBoxLayout * left_lt = new QVBoxLayout;
	left_lt->addWidget(mcu_grp);
	left_lt->addStretch();
	left_lt->addWidget(actions_grp);
	
	QHBoxLayout * right_bottom_lt = new QHBoxLayout;
	right_bottom_lt->addWidget(pgm_stat_grp);
	right_bottom_lt->addWidget(msg_grp);
	
	QVBoxLayout * right_lt = new QVBoxLayout;
	right_lt->addWidget(flash_grp);
	right_lt->addLayout(right_bottom_lt);
	
	QHBoxLayout * main_lt = new QHBoxLayout;
	main_lt->addLayout(left_lt);
	main_lt->addLayout(right_lt);
	
	setLayout(main_lt);

    connect(mcu_family_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(change_mcu_list_slot(int)));
    connect(mcu_name_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(change_mcu_info_slot(int)));
    connect(open_flash_button,SIGNAL(clicked()),this,SLOT(open_flash_slot()));
    connect(this,SIGNAL(update_msg_browser_signal(QString)),this,SLOT(update_msg_browser_slot(QString)));
    connect(go_button,SIGNAL(clicked()),this,SLOT(go_button_slot()));
    connect(this,SIGNAL(show_flash_signal(OP_MEM_TYPE)),this,SLOT(show_flash_slot(OP_MEM_TYPE)));
    connect(erase_button,SIGNAL(clicked()),this,SLOT(erase_signal_slot()));
}

ActionWidget :: ~ActionWidget() {}


void ActionWidget :: erase_signal_slot()
{
    if(mcu_family_combo->currentIndex() >=0 && mcu_name_combo->currentIndex()>=0)
    {
        emit erase_signal(mcu_family_combo->currentIndex(),mcu_name_combo->currentIndex());
    }
}

void ActionWidget :: change_pgm_status_slot(bool status,QString info)
{
	if(status)
	{
        pgm_status_label->setText("Programmer Found "+info);
		pgm_status_icon_label->setPixmap(QPixmap(GREEN_ICON));
	}else
	{
        pgm_status_label->setText("Programmer Not Found");
		pgm_status_icon_label->setPixmap(QPixmap(RED_ICON));
	}
}

void ActionWidget :: change_mcu_list_slot(int family_index)
{
    mcu_name_combo->clear();
    for(vector<mcu_info_t*>::iterator it=mapped_mcus[family_index].begin();it!=mapped_mcus[family_index].end();it++)
        mcu_name_combo->addItem((*it)->name);
    mcu_family_label->setText(QString(MCU_FAMILY_LABEL)+mcu_family_combo->currentText());
    emit update_msg_browser_signal(QString("New Chip family Selected <")+mcu_family_combo->currentText()+QString(">"));
    if(family_index >=0 && mcu_name_combo->currentIndex() >= 0)
        emit update_about_mcu_signal(mcu_family_combo->currentIndex(),mcu_name_combo->currentIndex());
}

void ActionWidget :: change_mcu_info_slot(int mcu_index)
{
    if(mcu_index >= 0)
    {
        mcu_info_t * info = mapped_mcus[mcu_family_combo->currentIndex()].at(mcu_index);
        mcu_name_label->setText(QString(MCU_NAME_LABEL)+info->name);
        emit update_about_mcu_signal(mcu_family_combo->currentIndex(),mcu_index);
        emit update_msg_browser_signal(QString("New Chip Selected <")+QString(info->name)+QString(">"));
    }
}

void ActionWidget :: open_flash_slot()
{
    int family_index = mcu_family_combo->currentIndex();
    int mcu_index = mcu_name_combo->currentIndex();
    if(family_index >=0 && mcu_index >=0)
    {
        mcu_info_t * info = mapped_mcus[family_index].at(mcu_index);
        QString file_name = QFileDialog::getOpenFileName(this,
                                                         "Open Flash Data",
                                                         "../../",
                                                         "HEX File(*.hex *.HEX)");
        if(!file_name.isEmpty())
        {
            qDebug()<<file_name<<" for "<<info->name;
            int ret = read_hex_data(opprog,file_name.toAscii(),info);
            qDebug()<<ret;
            if(ret)
            {
                QMessageBox::warning(this,"Error","Could Not read Flash Data");
            }else
            {
                qDebug()<< (int)opprog->memory->flash_loaded->len;
                qDebug()<< (int)opprog->memory->eeprom_loaded->len;
                qDebug()<< (int)opprog->memory->config_loaded->len;

                emit show_flash_signal(FLASH_LOADED);
                if(opprog->memory->config_loaded->len > 0)
                    emit update_config_viewer_signal(mcu_family_combo->currentIndex(),CONFIG_LOADED);
                if(opprog->memory->eeprom_loaded->len > 0)
                    emit update_config_viewer_signal(mcu_family_combo->currentIndex(),EEPROM_LOADED);
                emit update_msg_browser_signal(QString::number((int)opprog->memory->flash_loaded->len)+QString(" Bytes of Flash Data loaded"));
            }
        }
    }else
    {
        QMessageBox::warning(this,"Error","Select Chip First");
    }
}

void ActionWidget :: update_msg_browser_slot(QString msg)
{
    msg_browser->append(msg);
}

void ActionWidget :: pgm_update_slot(const char *sender, const char *msg, int val)
{
    //qDebug()<<sender<<" : " << msg<<" : "<<val;
    if(strcasecmp(msg,"progress") == 0)
    {
        if(val == 0)
        {
            progress_bar->setVisible(true);
        }else if(val == 100)
        {
            progress_bar->setVisible(false);
        }else
            progress_bar->setValue(val);
    }else if(strcasecmp(msg,"show_flash_read") == 0)
    {
        emit show_flash_signal(FLASH_READ);
    }else if(strcasecmp(msg,"show_config_read") == 0)
    {
        emit update_config_viewer_signal(mcu_family_combo->currentIndex(),CONFIG_READ);
    }else
    {
        msg_browser->append(QString(sender)+" : "+QString(msg));
    }
}


void ActionWidget :: go_button_slot()
{
    if(mcu_family_combo->currentIndex()>=0 && mcu_name_combo->currentIndex()>=0 )
    {
        setup_operations();
        qDebug()<<"LAST FLASH ACTION : "<< last_flash_action;
        qDebug()<<"LAST EEPROM ACTION : "<< last_eeprom_action;
        qDebug()<<"LAST CONFIG ACTION : "<< last_config_action;
        if(read_first->isChecked())
        {
            qDebug()<<"Read first";
            emit go_button_signal(true,mcu_family_combo->currentIndex(),mcu_name_combo->currentIndex());
        }else
        {
            emit go_button_signal(false,mcu_family_combo->currentIndex(),mcu_name_combo->currentIndex());
        }
    }
}


void ActionWidget :: setup_operations()
{
    mcu_info_t * info = mapped_mcus[mcu_family_combo->currentIndex()].at(mcu_name_combo->currentIndex());
    int count = 0;
    if(read_flash_chkbox->isChecked())
    {
        count++;
        if(strcasecmp(info->family,mcu_families[ATMEL].name) == 0)
        {
            count++;//for calibration bytes
        }
    }
    if(read_eeprom_chkbox->isChecked()) count++;
    if(read_config_chkbox->isChecked())
    {
        count++;
        if(strcasecmp(info->family,mcu_families[ATMEL].name) == 0)
        {
            if(avr_read_config_operations.read_lock) count++;//for config bytes
            if(avr_read_config_operations.read_lfuse) count++;//for config bytes
            if(avr_read_config_operations.read_hfuse) count++;//for config bytes
            if(avr_read_config_operations.read_efuse) count++;//for config bytes
        }
    }
    if(write_flash_chkbox->isChecked()) count++;
    if(write_eeprom_chkbox->isChecked()) count++;
    if(write_config_chkbox->isChecked())
    {
        count++;
        if(strcasecmp(info->family,mcu_families[ATMEL].name) == 0)
        {
            if(avr_read_config_operations.write_lock) count++;//for config bytes
            if(avr_read_config_operations.write_lfuse) count++;//for config bytes
            if(avr_read_config_operations.write_hfuse) count++;//for config bytes
            if(avr_read_config_operations.write_efuse) count++;//for config bytes
        }
    }

    qDebug()<<memory_options.len << " " << count;
    if(count > 0)
    {
        OP_MEM_TYPE * types = (OP_MEM_TYPE *)realloc(memory_options.types,count*sizeof(OP_MEM_TYPE));
        if(types)
        {
            memory_options.types = types;
            memory_options.len = count;
            memset(memory_options.types,0,count*sizeof(OP_MEM_TYPE));

            count = 0;
            if(read_flash_chkbox->isChecked())
            {
                memory_options.types[count++] = FLASH_READ;
                if(strcasecmp(info->family,mcu_families[ATMEL].name) == 0)
                {
                    memory_options.types[count++] = AVR_CAL;
                }
            }
            if(read_eeprom_chkbox->isChecked()) memory_options.types[count++] = EEPROM_READ;
            if(read_config_chkbox->isChecked())
            {
                memory_options.types[count++] = CONFIG_READ;
                if(strcasecmp(info->family,mcu_families[ATMEL].name) == 0)
                {
                    if(avr_read_config_operations.read_lock) memory_options.types[count++] = AVR_LOCK_READ;
                    if(avr_read_config_operations.read_lfuse) memory_options.types[count++] = AVR_LFUSE_READ;
                    if(avr_read_config_operations.read_hfuse) memory_options.types[count++] = AVR_HFUSE_READ;
                    if(avr_read_config_operations.read_efuse) memory_options.types[count++] = AVR_EFUSE_READ;
                }
            }
            if(write_flash_chkbox->isChecked())
            {
                memory_options.types[count++] = FLASH_TO_WRITE;
                if(last_flash_action == LAST_FLASH_LOADED)
                {
                    opprog->clear_memory(opprog,FLASH_TO_WRITE);
                    opprog->append_data(opprog,FLASH_TO_WRITE,opprog->memory->flash_loaded->data,opprog->memory->flash_loaded->len);
                }else if(last_flash_action == LAST_FLASH_READ)
                {
                    opprog->clear_memory(opprog,FLASH_TO_WRITE);
                    opprog->append_data(opprog,FLASH_TO_WRITE,opprog->memory->flash_read->data,opprog->memory->flash_read->len);
                }
            }
            if(write_eeprom_chkbox->isChecked()) memory_options.types[count++] = EEPROM_TO_WRITE;
            if(write_config_chkbox->isChecked())
            {
                memory_options.types[count++] = CONFIG_TO_WRITE;
                if(strcasecmp(info->family,mcu_families[ATMEL].name) == 0)
                {
                    if(avr_read_config_operations.write_lock) memory_options.types[count++] = AVR_LOCK_WRITE;
                    if(avr_read_config_operations.write_lfuse) memory_options.types[count++] = AVR_LFUSE_WRITE;
                    if(avr_read_config_operations.write_hfuse) memory_options.types[count++] = AVR_HFUSE_WRITE;
                    if(avr_read_config_operations.write_efuse) memory_options.types[count++] = AVR_EFUSE_WRITE;
                }else if(strcasecmp(info->family,mcu_families[EEPROM].name) == 0)
                {}else
                {
                    if(last_config_action == LAST_CONFIG_LOADED)
                    {
                        opprog->clear_memory(opprog,CONFIG_TO_WRITE);
                        opprog->append_data(opprog,CONFIG_TO_WRITE,opprog->memory->config_loaded->data,opprog->memory->config_loaded->len);
                    }else if(last_config_action == LAST_CONFIG_READ)
                    {
                        opprog->clear_memory(opprog,CONFIG_TO_WRITE);
                        opprog->append_data(opprog,CONFIG_TO_WRITE,opprog->memory->config_read->data,opprog->memory->config_read->len);
                    }
                }
            }
        }else // memory trouble
        {
            memory_options.types = NULL;
            memory_options.len = 0;
        }
    }else // no operations
    {
        memory_options.types = NULL;
        memory_options.len = 0;
    }
    qDebug()<<memory_options.len << " " << count;
}

void ActionWidget :: show_flash_slot(OP_MEM_TYPE type)
{
    op_buffer_t * buffer = NULL;
    if(type == FLASH_LOADED)
    {
        buffer = opprog->memory->flash_loaded;
        last_flash_action = LAST_FLASH_LOADED;
    }else if(type == FLASH_READ)
    {
        buffer = opprog->memory->flash_read;
        last_flash_action = LAST_FLASH_READ;
    }
    if(buffer)
    {
        flash_viewer_table->clear();
        int bytes = (int)buffer->len;
        int number_of_rows;
        if((bytes%128)>0)
            number_of_rows = (bytes/128)+1;
        else
            number_of_rows = (bytes/128);
        int b_index = 0;
        flash_viewer_table->setRowCount(number_of_rows);
        for(int r = 0; r< number_of_rows;r++)
        {
            for(int c = 0; c< 128; c++)
            {
                if(b_index < bytes)
                {
                    QTableWidgetItem * item = new QTableWidgetItem;
                    item->setText(QString::number(buffer->data[b_index++],16));
                    flash_viewer_table->setItem(r,c,item);
                }else
                    break;
            }
        }
    }
}
