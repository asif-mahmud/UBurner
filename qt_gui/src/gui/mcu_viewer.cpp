#include <mcu_viewer.hpp>
#include <vector>

using namespace std;

McuDrawer ::McuDrawer(QWidget *parent) :QGraphicsScene(parent)
{
    setSceneRect(-(SCENE_WIDTH/2),-(SCENE_HEIGHT/2),SCENE_WIDTH,SCENE_HEIGHT);
    setBackgroundBrush(QColor("#191616"));
    draw_pdip_mcu(40);
}

McuDrawer :: ~McuDrawer() {}


void McuDrawer :: draw_axis()
{
    QPen pen(QColor("#ADA1A1"));
    setSceneRect(-(SCENE_WIDTH/2),-(SCENE_HEIGHT/2),SCENE_WIDTH,SCENE_HEIGHT);
    addLine(-(SCENE_WIDTH/2),0,(SCENE_WIDTH/2),0,pen);
    addLine(0,(SCENE_HEIGHT/2),0,-(SCENE_HEIGHT/2),pen);
}


void McuDrawer ::draw_pdip_mcu(int pin_cnt)
{
    clear();
    //draw_axis();
    int pin_w = 15;
    int pin_h = 14;
    int scene_h_offset = 20;
    int pin_h_offset = 2*pin_h;
    int pin_h_extra_offset = pin_h/2;
    int mcu_h = pin_h_extra_offset*2+((pin_cnt/2)-1)*pin_h_offset+pin_h;
    int mcu_w;
    if(pin_cnt <= 8) mcu_w = 180;
    else if(pin_cnt>8 && pin_cnt <=28) mcu_w = 260;
    else mcu_w = 340;
    int arc_w = 40;
    int arc_h = 2*pin_h;
    QPen pen(QColor("#EFEED4"));
    pen.setWidth(2);
    /**Set Scene size**/
    setSceneRect(-(mcu_w),-(mcu_h/2)-scene_h_offset,mcu_w*2,mcu_h+(2*scene_h_offset));
    /**Draw MCU body**/
    addRect(-(mcu_w/2),-(mcu_h/2),mcu_w,mcu_h,pen);
    /**Draw Mcu name**/
    QGraphicsTextItem * mcu_name = new QGraphicsTextItem;
    mcu_name->setPlainText("ATmega32A");
    mcu_name->setPos(-20,80);
    mcu_name->setScale(2);
    mcu_name->setDefaultTextColor(QColor("#8FB396"));
    mcu_name->setRotation(-90);
    this->addItem(mcu_name);
    /**Draw the curve on top**/
    QPainterPath arcPath;
    arcPath.arcMoveTo(-(arc_w/2),-(mcu_h/2)-(arc_h/2),arc_w,arc_h,0);
    arcPath.arcTo(-(arc_w/2),-(mcu_h/2)-(arc_h/2),arc_w,arc_h,0, -180);
    addPath(arcPath,pen);
    /**Draw the pins**/
    QPen txt_pen(QColor("#C8BE67"));
    for(int i = 0;i<(pin_cnt/2);i++)
    {
        addRect(-(mcu_w/2)-pin_w,-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pen);
        QGraphicsTextItem * left_pin= new QGraphicsTextItem;
        left_pin->setPos(-(mcu_w/2),-(mcu_h/2)+(i*pin_h_offset));
        left_pin->setPlainText(QString::number(i+1));
        left_pin->setDefaultTextColor(QColor("#C8BE67"));
        this->addItem(left_pin);
        addRect((mcu_w/2),-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pen);
        QGraphicsTextItem * right_pin= new QGraphicsTextItem;
        right_pin->setPos((mcu_w/2)-24,-(mcu_h/2)+(i*pin_h_offset));
        right_pin->setPlainText(QString::number(pin_cnt-i));
        right_pin->setDefaultTextColor(QColor("#C8BE67"));
        this->addItem(right_pin);
    }
}

void McuDrawer :: draw_pdip_mcu(mcu_info_t *info, vector<int>& pin_numbers, vector<QString>& pin_names)
{
    clear();
    int pin_cnt = info->package.pin_count;
    //draw_axis();
    int pin_w = 15;
    int pin_h = 14;
    int scene_h_offset = 20;
    int pin_h_offset = 2*pin_h;
    int pin_h_extra_offset = pin_h/2;
    int mcu_h = pin_h_extra_offset*2+((pin_cnt/2)-1)*pin_h_offset+pin_h;
    int mcu_w;
    if(pin_cnt <= 8) mcu_w = 180;
    else if(pin_cnt>8 && pin_cnt <=28) mcu_w = 260;
    else mcu_w = 340;
    int arc_w = 40;
    int arc_h = 2*pin_h;
    QPen pen(QColor("#EFEED4"));
    pen.setWidth(2);
    /**Set Scene size**/
    setSceneRect(-(mcu_w),-(mcu_h/2)-scene_h_offset,mcu_w*2,mcu_h+(2*scene_h_offset));
    /**Draw MCU body**/
    addRect(-(mcu_w/2),-(mcu_h/2),mcu_w,mcu_h,pen);
    /**Draw Mcu name**/
    QGraphicsTextItem * mcu_name = new QGraphicsTextItem;
    mcu_name->setPlainText(QString(info->name));
    mcu_name->setPos(-20,80);
    mcu_name->setScale(2);
    mcu_name->setDefaultTextColor(QColor("#8FB396"));
    mcu_name->setRotation(-90);
    this->addItem(mcu_name);
    /**Draw the curve on top**/
    QPainterPath arcPath;
    arcPath.arcMoveTo(-(arc_w/2),-(mcu_h/2)-(arc_h/2),arc_w,arc_h,0);
    arcPath.arcTo(-(arc_w/2),-(mcu_h/2)-(arc_h/2),arc_w,arc_h,0, -180);
    addPath(arcPath,pen);
    /**Draw the pins**/
    QPen txt_pen(QColor("#C8BE67"));
    QPen pgm_pin_pen(QColor("#ff09c2"));
    for(int i = 0;i<(pin_cnt/2);i++)
    {
        pin_map_t * map = info->pin_map;
        int number = 0;
        for(int pi = 0; pi < pin_names.size(); pi++)
        {
            if(pin_numbers[pi] == (i+1))
            {
                qDebug()<<"Left Pin " <<pin_numbers[pi];
                number = pin_numbers[pi];
                addRect(-(mcu_w/2)-pin_w,-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pgm_pin_pen);
                QGraphicsTextItem * left_pin= new QGraphicsTextItem;
                left_pin->setPos(-(mcu_w/2),-(mcu_h/2)+(i*pin_h_offset));
                left_pin->setPlainText(QString::number(number));
                left_pin->setDefaultTextColor(QColor("#06ff00"));
                QGraphicsTextItem * left_pin_txt= new QGraphicsTextItem;
                left_pin_txt->setPos(-(mcu_w/2)-pin_w-60,-(mcu_h/2)+(i*pin_h_offset));
                left_pin_txt->setPlainText(pin_names[pi]);
                left_pin_txt->setScale(1.5);
                left_pin_txt->setDefaultTextColor(QColor("#06ff00"));
                this->addItem(left_pin);
                this->addItem(left_pin_txt);
            }else if(pin_numbers[pi] == (pin_cnt-i))
            {
                qDebug()<<"Right Pin "<<pin_numbers[pi];
                number = pin_numbers[pi];
                addRect((mcu_w/2),-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pgm_pin_pen);
                QGraphicsTextItem * right_pin= new QGraphicsTextItem;
                right_pin->setPos((mcu_w/2)-24,-(mcu_h/2)+(i*pin_h_offset));
                right_pin->setPlainText(QString::number(number));
                right_pin->setDefaultTextColor(QColor("#06ff00"));
                QGraphicsTextItem * right_pin_txt= new QGraphicsTextItem;
                right_pin_txt->setPos((mcu_w/2)+pin_w+5,-(mcu_h/2)+(i*pin_h_offset));
                right_pin_txt->setPlainText(pin_names[pi]);
                right_pin_txt->setScale(1.5);
                right_pin_txt->setDefaultTextColor(QColor("#06ff00"));
                this->addItem(right_pin);
                this->addItem(right_pin_txt);
            }else
                qDebug()<<"NC "<<i<<":"<<pin_numbers[pi];
        }
        if(number == 0)
        {
            addRect(-(mcu_w/2)-pin_w,-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pen);
            QGraphicsTextItem * left_pin= new QGraphicsTextItem;
            left_pin->setPos(-(mcu_w/2),-(mcu_h/2)+(i*pin_h_offset));
            left_pin->setPlainText(QString::number(i+1));
            left_pin->setDefaultTextColor(QColor("#C8BE67"));
            this->addItem(left_pin);
            addRect((mcu_w/2),-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pen);
            QGraphicsTextItem * right_pin= new QGraphicsTextItem;
            right_pin->setPos((mcu_w/2)-24,-(mcu_h/2)+(i*pin_h_offset));
            right_pin->setPlainText(QString::number(pin_cnt-i));
            right_pin->setDefaultTextColor(QColor("#C8BE67"));
            this->addItem(right_pin);
        }else
        {
            if(number == (i+1))
            {
                addRect((mcu_w/2),-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pen);
                QGraphicsTextItem * right_pin= new QGraphicsTextItem;
                right_pin->setPos((mcu_w/2)-24,-(mcu_h/2)+(i*pin_h_offset));
                right_pin->setPlainText(QString::number(pin_cnt-i));
                right_pin->setDefaultTextColor(QColor("#C8BE67"));
                this->addItem(right_pin);
            }else if(number == (pin_cnt-i))
            {
                addRect(-(mcu_w/2)-pin_w,-(mcu_h/2)+(i*pin_h_offset)+pin_h_extra_offset,pin_w,pin_h,pen);
                QGraphicsTextItem * left_pin= new QGraphicsTextItem;
                left_pin->setPos(-(mcu_w/2),-(mcu_h/2)+(i*pin_h_offset));
                left_pin->setPlainText(QString::number(i+1));
                left_pin->setDefaultTextColor(QColor("#C8BE67"));
                this->addItem(left_pin);
            }
        }
    }
}

McuViewer ::McuViewer(QWidget *parent) :QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    drawer = new McuDrawer(this);
    setScene(drawer);
}

McuViewer :: ~McuViewer() {}

void McuViewer ::resizeEvent(QResizeEvent * e)
{
    this->fitInView(drawer->sceneRect());
}


void McuViewer :: update_mcu_drawer_slot(int family, int mcu)
{
    mcu_info_t * info = mapped_mcus[family].at(mcu);
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
    drawer->draw_pdip_mcu(info,pin_numbers,pin_names);
}
