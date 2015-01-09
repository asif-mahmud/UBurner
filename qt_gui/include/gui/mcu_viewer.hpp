#ifndef MCU_VIEWER_HPP
#define MCU_VIEWER_HPP

#include <QtGui>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <opprog_int.hpp>

#define SCENE_WIDTH     480
#define SCENE_HEIGHT    640

class McuDrawer : public QGraphicsScene
{
        Q_OBJECT

    public :
        McuDrawer(QWidget * parent = 0);
        virtual ~McuDrawer();

        void draw_axis();
    public :
        void draw_pdip_mcu(int pin_cnt);
        void draw_pdip_mcu(mcu_info_t *,std::vector<int>&,std::vector<QString>&);
};

class McuViewer : public QGraphicsView
{
        Q_OBJECT

    public :
        McuViewer(QWidget * parent = 0);
        virtual ~McuViewer();

    private :
        McuDrawer * drawer;
        void resizeEvent(QResizeEvent * );
    public slots :
        void update_mcu_drawer_slot(int,int);
};



#endif // MCU_VIEWER_HPP
