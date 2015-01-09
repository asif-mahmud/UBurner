#ifndef OP_GUI_PGM_FINDER_HPP
#define OP_GUI_PGM_FINDER_HPP

#include <QtGui>

class ActionThread:public QThread
{
        Q_OBJECT
    public:
        ActionThread();
        virtual ~ActionThread();

    protected:
        void run();
        bool search;
        bool is_opened;
        bool to_perform_mem_actions;
        bool read_first;
        bool run_erase;

        void do_read_operations();
        void do_write_operations();

    public:
        void start_searching();
        void stop_searching();

    public slots:
        void perform_mem_actions(bool,int,int);//family,mcu indexes
        void erase_slot(int,int);

    signals :
        void programmer_status_changed(bool,QString);
};

#endif
