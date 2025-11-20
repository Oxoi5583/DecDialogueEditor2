#include "obj/abstract/dragable.h"
#include "graph/grid.h"
#include "graph/selection.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include <cmath>


DragableObject::DragableObject(){
    BIND_CLASS(DragableObject);
}
DragableObject::~DragableObject(){

}

void DragableObject::m_update_state(){
    switch (m_current_state) {
        case State::IDLE:{
            if(this->was_just_clicked()){
                drag_ready();
            }
            break;
        }
        case State::READY:{
            vec2 new_mouse_pos = MouseServer::Ref()->get_mouse_world_position();
            if(m_ready_mouse_pos != new_mouse_pos){
                drag();
                break;
            }
            if(this->was_just_released()){
                m_current_state = State::RESTORE_IDLE;
            }
            break;
        }
        case State::RESTORE_IDLE:{
            m_current_state = State::IDLE;
            break;
        }
        case State::DRAG:{
            if(this->was_just_released()){
                place();
            }
            break;
        }
        case State::PLACE:{
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
        case State::RESTORE_IDLE:{
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
        case State::PLACE:{
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
    m_handle_action();
    m_update_state();
    m_emit_event();
}
void DragableObject::process(){

}
void DragableObject::post_process(){

}
void DragableObject::draw(){

}

bool DragableObject::is_restore_to_idle(){
    return m_current_state == State::RESTORE_IDLE;
}
bool DragableObject::is_drag_ready(){
    return m_current_state == State::READY;
}
bool DragableObject::is_dragging(){
    return m_current_state == State::DRAG;
}
bool DragableObject::is_placed(){
    return m_current_state == State::PLACE;
}


void DragableObject::drag_ready(){
    if(!is_drag_ready()){
        m_current_state = State::READY;
        m_ready_mouse_pos = MouseServer::Ref()->get_mouse_world_position();
    }
}
void DragableObject::drag(){
    if(!is_dragging()){
        m_dragging_position_offset = MouseServer::Ref()->get_mouse_world_position() - this->get_position();
        m_current_state = State::DRAG;
    }
}
void DragableObject::place(){
    if(!is_placed()){
        m_current_state = State::PLACE;
        m_align_grid();
    }
}

void DragableObject::m_align_grid(){
    vec2 pos = this->get_position();
    double x = pos.x;
    double y = pos.y;
    x = roundf(x / GraphGrid::Ref()->grid_interval) * GraphGrid::Ref()->grid_interval;
    y = roundf(y / GraphGrid::Ref()->grid_interval) * GraphGrid::Ref()->grid_interval;

    this->set_position({x,y});
}

void DragableObject::m_emit_event(){
    if(is_dragging()){
        EventMouseDragObj event;
        event.obj_id = get_id();
        EventServer::Ref()->emit(event);
    }
    if(is_placed()){
        EventMousePlaceObj event;
        event.obj_id = get_id();
        EventServer::Ref()->emit(event);
    }
}
