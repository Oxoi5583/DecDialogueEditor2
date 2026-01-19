#include "system/obj/abstract/clickable.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"


ClickableObject::ClickableObject(){
    BIND_CLASS(ClickableObject);
    m_double_click_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::MILLISECOND, 250) ,false);
    m_double_click_timer->stop();
}
ClickableObject::~ClickableObject(){

}

void ClickableObject::ready(){
    
}
void ClickableObject::pre_process(){
    m_was_just_clicked = false;
    m_was_just_released = false;

    if(EventServer::Ref()->has<EventLockedAll>()){
        m_was_clicked = false;
        return;
    }

    if(this->was_hovered()){
        if(MouseServer::Ref()->is_just_clicked()){
            m_was_just_clicked = true;
            m_was_clicked = true;
            ObjectServer::Ref()->move_to_front(this->get_id());

            m_click_times += 1;
            if(!m_double_click_timer->is_timeout()){
                m_double_click_timer->reset();
                m_double_click_timer->start();
            }
        }

        if(m_double_click_timer->is_timeout()){
            m_click_times = 0;
            m_double_click_timer->stop();
            m_double_click_timer->reset();
        }
    }

    if(MouseServer::Ref()->is_just_released()){
        m_was_just_released = true;
        m_was_clicked = false;
    }
    m_emit_event();
}
void ClickableObject::process(){

}
void ClickableObject::post_process(){

}
void ClickableObject::draw(){

}


bool ClickableObject::was_just_double_clicked(){
    return (m_click_times == 2);
}
bool ClickableObject::was_just_clicked(){
    return m_was_just_clicked;
}
bool ClickableObject::was_clicked(){
    return m_was_clicked;
}
bool ClickableObject::was_just_released(){
    return m_was_just_released;
}


void ClickableObject::m_emit_event(){
    ObjectServer::Layer layer = (ObjectServer::Layer)this->get_layer();

    if(was_just_clicked()){
        EventMouseJustClickObj event;
        event.obj_id = this->get_id();
        event.click_pos = MouseServer::Ref()->get_mouse_world_position();
        event.layer = layer;
        EventServer::Ref()->emit(event);
    }
    if(was_clicked()){
        EventMouseClickObj event;
        event.obj_id = this->get_id();
        event.click_pos = MouseServer::Ref()->get_mouse_world_position();
        event.layer = layer;
        EventServer::Ref()->emit(event);
    }
}