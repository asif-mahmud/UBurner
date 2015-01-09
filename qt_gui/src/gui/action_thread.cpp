#include <action_thread.hpp>
#include <opprog_int.hpp>

mcu_info_t * action_mcu_info;

ActionThread ::ActionThread()
{
    search = false;
    is_opened = false;
    to_perform_mem_actions = false;
    read_first = true;
    run_erase = false;
}

ActionThread :: ~ActionThread() {}

void ActionThread :: start_searching()
{
    search = true;
    is_opened = false;
    to_perform_mem_actions = false;
    read_first = true;
    run_erase = false;
    start();
}

void ActionThread :: stop_searching()
{
    search = false;
}

void ActionThread :: run()
{
    while(search)
    {
        if(!to_perform_mem_actions && !run_erase)
        {
            int found = opprog->find_device(0x04D8,0x0100);

            if(found && !is_opened)
            {
                //open it
                opprog->open_device(opprog,0x4D8,0x0100);
                if(opprog->dev_hnd)
                {
                    QString info;
                    info += "\nVID : 0x"+QString::number(opprog->dev_info->vid,16);
                    info += "\nPID : 0x"+QString::number(opprog->dev_info->pid,16);
                    info += "\nManufacturer String : "+QString::fromWCharArray(opprog->dev_info->manufacturer_string);
                    info += "\nProduct String : "+QString::fromWCharArray(opprog->dev_info->product_string);
                    info += "\nIndexed String : "+QString::fromWCharArray(opprog->dev_info->indexed_string);
                    info += "\nSerial Number String : "+QString::fromWCharArray(opprog->dev_info->serial_number_string);
                    is_opened = true;
                    emit programmer_status_changed(true,info);
                }
            }else if(!found && is_opened)
            {
                hid_close(opprog->dev_hnd);
                is_opened = false;
                emit programmer_status_changed(false,"Not Found");
            }
        }else if(!to_perform_mem_actions && run_erase)
        {
            programmer_t * pgm = find_programmer_by_family(pgms,action_mcu_info->family);
            if(pgm)
            {                
                qDebug()<< pgm->erase(opprog,action_mcu_info,NULL);
                qDebug()<<"Erasing ..."<< pgm->name;
            }
            run_erase = false;
        }else
        {
            if(read_first)
            {
                do_read_operations();
                do_write_operations();
            }else
            {
                do_write_operations();
                do_read_operations();
            }
            to_perform_mem_actions = false;
        }
    }
}

void ActionThread :: perform_mem_actions(bool read_first,int family, int mcu)
{
    if(memory_options.len > 0)
    {
        action_mcu_info = mapped_mcus[family].at(mcu);
        this->read_first = read_first;
        to_perform_mem_actions = true;
        run_erase = false;
    }
}

void ActionThread :: do_read_operations()
{
    programmer_t * pgm = find_programmer_by_family(pgms,action_mcu_info->family);

    if(pgm)
    {
        qDebug()<<"Selected PGM (READ) : "<<pgm->name;
        qDebug()<<"Out From Reading : "<<pgm->read(opprog,action_mcu_info,&memory_options);
    }
}

void ActionThread :: do_write_operations()
{
    programmer_t * pgm = find_programmer_by_family(pgms,action_mcu_info->family);

    if(pgm)
    {
        qDebug()<<"Selected PGM (WRITE): "<<pgm->name;
        qDebug()<<"Out from Writing : "<<pgm->write(opprog,action_mcu_info,&memory_options);;
    }
}

void ActionThread :: erase_slot(int family, int mcu)
{
    mcu_info_t * info = mapped_mcus[family].at(mcu);
    programmer_t * pgm = find_programmer_by_family(pgms,info->family);
    if(pgm != NULL)
    {
        action_mcu_info = info;
        run_erase = true;
        to_perform_mem_actions = false;
    }
}
