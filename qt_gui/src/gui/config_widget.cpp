#include <config_widget.hpp>
#include <QtGui>

char PIC_CONFIG_NAMES[PIC_CONFIG_NAME_SIZE][PIC_CONFIG_NAME_LEN] = {
    {"ID 0"},
    {"ID 1"},
    {"ID 2"},
    {"ID 3"},
    {"ID 4"},
    {"ID 5"},
    {"ID 6"},
    {"ID 7"},
    {"CONFIG 1L"},
    {"CONFIG 1H"},
    {"CONFIG 2L"},
    {"CONFIG 2H"},
    {"CONFIG 3L"},
    {"CONFIG 3H"},
    {"CONFIG 4L"},
    {"CONFIG 4H"},
    {"CONFIG 5L"},
    {"CONFIG 5H"},
    {"CONFIG 6L"},
    {"CONFIG 6H"},
    {"CONFIG 7L"},
    {"CONFIG 7H"}
};

ConfigWidget :: ConfigWidget(QWidget * parent) : QDialog(parent)
{    
	QGroupBox * eeprom_box = new QGroupBox(EEPROM_BOX);

    avr_config = new QGroupBox("Atmel Configurations");
    avr_read_lock = new QCheckBox("Read Lock");
    avr_read_lfuse = new QCheckBox("Read Low Fuse");
    avr_read_hfuse = new QCheckBox("Read High Fuse");
    avr_read_efuse = new QCheckBox("Read Extented\nFuse");
    avr_write_lock = new QCheckBox("Write Lock");
    avr_write_lfuse = new QCheckBox("Write Low Fuse");
    avr_write_hfuse = new QCheckBox("Write High Fuse");
    avr_write_efuse = new QCheckBox("Write Extented\nFuse");
    avr_lock_edit = new QLineEdit;
    QRegExp re("[_a-fA-F0-9][_a-fA-F0-9]+");
    QRegExpValidator *v = new QRegExpValidator(re);
    avr_lock_edit->setValidator(v);
    avr_lock_edit->setMaxLength(2);
    avr_lfuse_edit = new QLineEdit;
    avr_lfuse_edit->setValidator(v);
    avr_lfuse_edit->setMaxLength(2);
    avr_hfuse_edit = new QLineEdit;
    avr_hfuse_edit->setValidator(v);
    avr_hfuse_edit->setMaxLength(2);
    avr_efuse_edit = new QLineEdit;
    avr_efuse_edit->setValidator(v);
    avr_efuse_edit->setMaxLength(2);
    QLabel * avr_lock = new QLabel("Lock Byte: (in HEX)");
    QLabel * avr_lfuse = new QLabel("Low Fuse Byte: (in HEX)");
    QLabel * avr_hfuse = new QLabel("High Fuse Byte: (in HEX)");
    QLabel * avr_efuse = new QLabel("Extended\nFuse Byte: (in HEX)");

    QVBoxLayout * avr_l_1 = new QVBoxLayout;
    avr_l_1->addWidget(avr_lock);
    avr_l_1->addWidget(avr_lfuse);
    avr_l_1->addWidget(avr_hfuse);
    avr_l_1->addWidget(avr_efuse);

    QVBoxLayout * avr_l_2 = new QVBoxLayout;
    avr_l_2->addWidget(avr_lock_edit);
    avr_l_2->addWidget(avr_lfuse_edit);
    avr_l_2->addWidget(avr_hfuse_edit);
    avr_l_2->addWidget(avr_efuse_edit);

    QVBoxLayout * avr_l_3 = new QVBoxLayout;
    avr_l_3->addWidget(avr_read_lock);
    avr_l_3->addWidget(avr_read_lfuse);
    avr_l_3->addWidget(avr_read_hfuse);
    avr_l_3->addWidget(avr_read_efuse);

    QVBoxLayout * avr_l_4 = new QVBoxLayout;
    avr_l_4->addWidget(avr_write_lock);
    avr_l_4->addWidget(avr_write_lfuse);
    avr_l_4->addWidget(avr_write_hfuse);
    avr_l_4->addWidget(avr_write_efuse);

    QHBoxLayout * avr_main_l = new QHBoxLayout;
    avr_main_l->addLayout(avr_l_1);
    avr_main_l->addLayout(avr_l_2);
    avr_main_l->addLayout(avr_l_3);
    avr_main_l->addLayout(avr_l_4);

    avr_config->setLayout(avr_main_l);

    pic_config = new QGroupBox("PIC Configuraions");
    QLabel * pic_cfg_lbl[PIC_CONFIG_NAME_SIZE];
    for(int i =0;i<PIC_CONFIG_NAME_SIZE;i++)
    {
        pic_cfg_lbl[i] = new QLabel(PIC_CONFIG_NAMES[i]);
        pic_config_viwer[i] = new QLineEdit;
        pic_config_viwer[i]->setMaxLength(2);
    }

    QVBoxLayout * pic_l_l_1 = new QVBoxLayout;
    QVBoxLayout * pic_l_l_2 = new QVBoxLayout;
    for(int i =0;i<8;i++)
    {
        pic_l_l_1->addWidget(pic_cfg_lbl[i]);
        pic_l_l_2->addWidget(pic_config_viwer[i]);
    }
    QHBoxLayout * pic_l = new QHBoxLayout;
    pic_l->addLayout(pic_l_l_1);
    pic_l->addLayout(pic_l_l_2);

    QVBoxLayout * pic_mid_1 = new QVBoxLayout;
    QVBoxLayout * pic_mid_2 = new QVBoxLayout;
    for(int i =0;i<7;i++)
    {
        pic_mid_1->addWidget(pic_cfg_lbl[i+8]);
        pic_mid_2->addWidget(pic_config_viwer[i+8]);
    }
    QHBoxLayout * pic_mid = new QHBoxLayout;
    pic_mid->addLayout(pic_mid_1);
    pic_mid->addLayout(pic_mid_2);

    QVBoxLayout * pic_r_l_1 = new QVBoxLayout;
    QVBoxLayout * pic_r_l_2 = new QVBoxLayout;
    for(int i =0;i<7;i++)
    {
        pic_r_l_1->addWidget(pic_cfg_lbl[i+8+7]);
        pic_r_l_2->addWidget(pic_config_viwer[i+8+7]);
    }
    QHBoxLayout * pic_r = new QHBoxLayout;
    pic_r->addLayout(pic_r_l_1);
    pic_r->addLayout(pic_r_l_2);

    QHBoxLayout * pic_ml = new QHBoxLayout;
    pic_ml->addLayout(pic_l);
    pic_ml->addLayout(pic_mid);
    pic_ml->addLayout(pic_r);

    pic_config->setLayout(pic_ml);
	
    QVBoxLayout * eeprom_lt = new QVBoxLayout;
    eeprom_viewer_table = new QTableWidget(0,64);
    eeprom_viewer_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    eeprom_viewer_table->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    eeprom_viewer_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStringList h_headers;
    for(int i=0;i<64;i++)
        h_headers.push_back(QString::number(i,16));
    eeprom_viewer_table->setHorizontalHeaderLabels(h_headers);
	eeprom_lt->addWidget(eeprom_viewer_table);
	eeprom_box->setLayout(eeprom_lt);
	
    QHBoxLayout * top_main_l = new QHBoxLayout;
    top_main_l->addWidget(avr_config);
    top_main_l->addWidget(pic_config);
    QVBoxLayout * main_lt = new QVBoxLayout;
    main_lt->addLayout(top_main_l);
	main_lt->addWidget(eeprom_box);
	
	setLayout(main_lt);


    connect(avr_read_lock,SIGNAL(clicked(bool)),this,SLOT(set_avr_read_lock(bool)));
    connect(avr_read_lfuse,SIGNAL(clicked(bool)),this,SLOT(set_avr_read_lfuse(bool)));
    connect(avr_read_hfuse,SIGNAL(clicked(bool)),this,SLOT(set_avr_read_hfuse(bool)));
    connect(avr_read_efuse,SIGNAL(clicked(bool)),this,SLOT(set_avr_read_efuse(bool)));
    connect(avr_write_lock,SIGNAL(clicked(bool)),this,SLOT(set_avr_write_lock(bool)));
    connect(avr_write_lfuse,SIGNAL(clicked(bool)),this,SLOT(set_avr_write_lfuse(bool)));
    connect(avr_write_hfuse,SIGNAL(clicked(bool)),this,SLOT(set_avr_write_hfuse(bool)));
    connect(avr_write_efuse,SIGNAL(clicked(bool)),this,SLOT(set_avr_write_efuse(bool)));
    connect(avr_lock_edit,SIGNAL(textChanged(QString)),this,SLOT(update_avr_write_lock(QString)));
    connect(avr_lfuse_edit,SIGNAL(textChanged(QString)),this,SLOT(update_avr_write_lfuse(QString)));
    connect(avr_hfuse_edit,SIGNAL(textChanged(QString)),this,SLOT(update_avr_write_hfuse(QString)));
    connect(avr_efuse_edit,SIGNAL(textChanged(QString)),this,SLOT(update_avr_write_efuse(QString)));
}

ConfigWidget :: ~ConfigWidget() {}

void ConfigWidget :: pgm_update_cb(const char * sender, const char *msg, int val)
{
    if(strcasecmp(msg,"show_eeprom_read") == 0)
    {
        //set the to_write buffer for future use
        show_eeprom_slot(EEPROM_READ);//always show read buffer here
    }
}

void ConfigWidget :: show_eeprom_slot(OP_MEM_TYPE type)
{
    op_buffer_t * buffer = NULL;
    if(type == EEPROM_LOADED)
    {
        buffer = opprog->memory->eeprom_loaded;
        last_eeprom_action = LAST_EEPROM_LOADED;
    }else if(type == EEPROM_READ)
    {
        buffer = opprog->memory->eeprom_read;
        last_eeprom_action = LAST_EEPROM_READ;
    }
    if(buffer)
    {
        eeprom_viewer_table->clear();
        int bytes = (int)buffer->len;
        int number_of_rows;
        if((bytes%64)>0)
            number_of_rows = (bytes/64)+1;
        else
            number_of_rows = (bytes/64);
        int b_index = 0;
        eeprom_viewer_table->setRowCount(number_of_rows);
        for(int r = 0; r< number_of_rows;r++)
        {
            for(int c = 0; c< 64; c++)
            {
                if(b_index < bytes)
                {
                    QTableWidgetItem * item = new QTableWidgetItem;
                    item->setText(QString::number(buffer->data[b_index++],16));
                    eeprom_viewer_table->setItem(r,c,item);
                }else
                    break;
            }
        }
    }
}

void ConfigWidget :: show_config_slot(int family, OP_MEM_TYPE type)
{
    qDebug()<<"Family : "<<family<<" MemType: "<<type;
    if(family == ATMEL)
    {
        if( type == CONFIG_READ)
        {
            last_config_action = LAST_CONFIG_READ;

            //always show to_write buffer
            avr_lock_edit->setText(QString::number(opprog->memory->config_read->data[AVR_LOCK_POS],16));
            avr_lfuse_edit->setText(QString::number(opprog->memory->config_read->data[AVR_LFUSE_POS],16));
            avr_hfuse_edit->setText(QString::number(opprog->memory->config_read->data[AVR_HFUSE_POS],16));
            avr_efuse_edit->setText(QString::number(opprog->memory->config_read->data[AVR_EFUSE_POS],16));
        }
    }else if(family == EEPROM)
    {

    }else
    {
        if( type == CONFIG_READ)
        {
            last_config_action = LAST_CONFIG_READ;

            for(int i =0; i< PIC_CONFIG_NAME_SIZE; i++)
            {
                pic_config_viwer[i]->setText(QString::number(opprog->memory->config_read->data[i],16));
            }
        }else if(type == CONFIG_LOADED)
        {
            last_config_action = LAST_CONFIG_LOADED;

            if(opprog->memory->config_loaded->len == 14)
            {
                for(int i = 0;i<PIC_CONFIG_NAME_SIZE-8;i++)
                    pic_config_viwer[i+8]->setText(QString::number(opprog->memory->config_loaded->data[i],16));
            }else if(opprog->memory->config_loaded->len == PIC_CONFIG_NAME_SIZE)
            {
                for(int i = 0;i<PIC_CONFIG_NAME_SIZE;i++)
                    pic_config_viwer[i]->setText(QString::number(opprog->memory->config_loaded->data[i],16));
            }

        }

        if(type == EEPROM_LOADED )
        {
            show_eeprom_slot(EEPROM_LOADED);
        }
    }
}

void ConfigWidget :: mcu_config_set_slot(int family, int mcu)
{}

void ConfigWidget :: set_avr_read_lock(bool state)
{
    if(state)
        avr_read_config_operations.read_lock = 1;
    else
        avr_read_config_operations.read_lock = 0;
}
void ConfigWidget :: set_avr_read_lfuse(bool state)
{
    if(state)
        avr_read_config_operations.read_lfuse = 1;
    else
        avr_read_config_operations.read_lfuse = 0;
}
void ConfigWidget :: set_avr_read_hfuse(bool state)
{
    if(state)
        avr_read_config_operations.read_hfuse = 1;
    else
        avr_read_config_operations.read_hfuse = 0;
}
void ConfigWidget :: set_avr_read_efuse(bool state)
{
    if(state)
        avr_read_config_operations.read_efuse = 1;
    else
        avr_read_config_operations.read_efuse = 0;
}

void ConfigWidget :: set_avr_write_lock(bool state)
{
    if(state)
    {
        avr_read_config_operations.write_lock = 1;
    }
    else
        avr_read_config_operations.write_lock = 0;
}
void ConfigWidget :: set_avr_write_lfuse(bool state)
{
    if(state)
    {
        avr_read_config_operations.write_lfuse = 1;
    }
    else
        avr_read_config_operations.write_lfuse = 0;
}
void ConfigWidget :: set_avr_write_hfuse(bool state)
{
    if(state)
    {
        avr_read_config_operations.write_hfuse = 1;
    }
    else
        avr_read_config_operations.write_hfuse = 0;
}
void ConfigWidget :: set_avr_write_efuse(bool state)
{
    if(state)
    {
        avr_read_config_operations.write_efuse = 1;
    }
    else
        avr_read_config_operations.write_efuse = 0;
}

void ConfigWidget :: update_avr_write_lock(QString txt)
{
    if(opprog->memory->config_to_write->len != AVR_CONFIG_SIZE)
    {
        opprog->clear_memory(opprog,CONFIG_TO_WRITE);
        UCHAR * tmp = (UCHAR *)calloc(AVR_CONFIG_SIZE,sizeof(UCHAR));
        opprog->memory->config_to_write->data = tmp;
        opprog->memory->config_to_write->len = AVR_CONFIG_SIZE;
    }
    opprog->memory->config_to_write->data[AVR_LOCK_POS] = avr_lock_edit->text().toUInt(0,16);
}

void ConfigWidget :: update_avr_write_lfuse(QString txt)
{
    if(opprog->memory->config_to_write->len != AVR_CONFIG_SIZE)
    {
        opprog->clear_memory(opprog,CONFIG_TO_WRITE);
        UCHAR * tmp = (UCHAR *)calloc(AVR_CONFIG_SIZE,sizeof(UCHAR));
        opprog->memory->config_to_write->data = tmp;
        opprog->memory->config_to_write->len = AVR_CONFIG_SIZE;
    }
    opprog->memory->config_to_write->data[AVR_LFUSE_POS] = avr_lfuse_edit->text().toUInt(0,16);
}

void ConfigWidget :: update_avr_write_hfuse(QString txt)
{
    if(opprog->memory->config_to_write->len != AVR_CONFIG_SIZE)
    {
        opprog->clear_memory(opprog,CONFIG_TO_WRITE);
        UCHAR * tmp = (UCHAR *)calloc(AVR_CONFIG_SIZE,sizeof(UCHAR));
        opprog->memory->config_to_write->data = tmp;
        opprog->memory->config_to_write->len = AVR_CONFIG_SIZE;
    }
    opprog->memory->config_to_write->data[AVR_HFUSE_POS] = avr_hfuse_edit->text().toUInt(0,16);
}

void ConfigWidget :: update_avr_write_efuse(QString txt)
{
    if(opprog->memory->config_to_write->len != AVR_CONFIG_SIZE)
    {
        opprog->clear_memory(opprog,CONFIG_TO_WRITE);
        UCHAR * tmp = (UCHAR *)calloc(AVR_CONFIG_SIZE,sizeof(UCHAR));
        opprog->memory->config_to_write->data = tmp;
        opprog->memory->config_to_write->len = AVR_CONFIG_SIZE;
    }
    opprog->memory->config_to_write->data[AVR_EFUSE_POS] = avr_efuse_edit->text().toUInt(0,16);
}
