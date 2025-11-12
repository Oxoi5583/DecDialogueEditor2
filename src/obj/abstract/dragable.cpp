#include "obj/abstract/dragable.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"


DragableObject::DragableObject(){
    BIND_CLASS(DragableObject);
}
DragableObject::~DragableObject(){

}

void DragableObject::m_update_state(){
    switch (m_current_state) {
        case State::IDLE:{
            if(this->was_just_clicked()){
                m_current_state = State::DRAG;
                m_ready_mouse_pos = MouseServer::Ref()->get_mouse_world_position();
            }
            break;
        }
        case State::READY:{
            vec2 new_mouse_pos = MouseServer::Ref()->get_mouse_world_position();
            if(m_ready_mouse_pos != new_mouse_pos){
                m_dragging_position_offset = MouseServer::Ref()->get_mouse_world_position() - this->get_position();
                m_current_state = State::DRAG;
                break;
            }
            if(this->was_just_released()){
                m_current_state = State::IDLE;
            }
            break;
        }
        case State::DRAG:{
            if(this->was_just_released()){
                m_current_state = State::PLEACE;
            }
            break;
        }
        case State::PLEACE:{
            m_current_state = State::IDLE;
            break;
        }
    }
}
void DragableObject::m_handle_action(){
    switch (m_current_state) {
        case State::IDLE:{
            break;
        }
        case State::READY:{
            break;
        }
        case State::DRAG:{
            vec2 mouse_pos = MouseServer::Ref()->get_mouse_world_position();
            vec2 new_pos = mouse_pos - m_dragging_position_offset;
            this->set_position(new_pos);

            EventMouseDragObj event;
            event.drag_pos = new_pos;
            event.obj_id = get_id();
            EventServer::Ref()->emit(event);
            break;
        }
        case State::PLEACE:{
            EventMousePlaceObj event;
            event.place_pos = get_position();
            event.obj_id = get_id();
            EventServer::Ref()->emit(event);
            break;
        }
    }
}


void DragableObject::ready(){

}
void DragableObject::pre_process(){
    m_update_state();
    m_handle_action();
}
void DragableObject::process(){

}
void DragableObject::post_process(){

}
void DragableObject::draw(){

}

bool DragableObject::is_dragging(){
    return m_current_state == State::DRAG;
}