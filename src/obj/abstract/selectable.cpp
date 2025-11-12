#include "obj/abstract/selectable.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/input_hub.h"
#include "engine/input_key.h"
#include "graph/selection.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "struct/shape/base.h"
#include "struct/shape/rect2.h"


SelectableObject::SelectableObject(){
    BIND_CLASS(SelectableObject);
}
SelectableObject::~SelectableObject(){

}

void SelectableObject::ready(){

}
void SelectableObject::pre_process(){
    m_update_state();
    m_emit_event();
}
void SelectableObject::process(){
}
void SelectableObject::post_process(){

}
void SelectableObject::draw(){

}

void SelectableObject::m_select(){
    m_state = State::SELECTED;
}
void SelectableObject::m_unselect(){
    m_state = State::IDLE;
}

void SelectableObject::m_update_state(){
    switch (m_state) {
        case IDLE:{
            m_select_if_clicked();
            m_select_if_in_select_area();
            break;
        }
        case SELECTED:{
            m_unselect_if_lshift_clicked();
            m_unselect_if_other_clicked();
            m_unselect_if_world_clicked();
            m_unselect_if_not_in_select_area();
            break;
        }
    }
}


void SelectableObject::m_select_if_clicked(){
    if(!this->is_on_camera()){
        return;
    }

    if(this->was_just_clicked()){
        m_select();
    }
}
void SelectableObject::m_select_if_in_select_area(){
    if(!this->is_on_camera()){
        return;
    }

    if(!GraphSelection::Ref()->is_selecting()){
        return;
    }

    Rect2 rect = {
        this->get_shape<ShapeBase>().get_position(),
        this->get_shape<ShapeBase>().get_size(),
    };

    if(!GraphSelection::Ref()->is_in_area(rect)){
        return;
    }

    m_select();
}

void SelectableObject::m_unselect_if_lshift_clicked(){
    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT) 
        && this->was_just_clicked()){
        m_unselect();
    }
}
void SelectableObject::m_unselect_if_other_clicked(){
    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)){
        return;
    }

    if(MouseServer::Ref()->is_just_clicked()
        && !this->was_just_clicked()
        && !EventServer::Ref()->has<EventMouseJustClickedOnWorld>()){
        m_unselect();
    }
}
void SelectableObject::m_unselect_if_world_clicked(){
    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)){
        return;
    }

    if(EventServer::Ref()->has<EventMouseJustClickedOnWorld>()){
        m_unselect();
    }
}
void SelectableObject::m_unselect_if_not_in_select_area(){
    if(!GraphSelection::Ref()->is_selecting()){
        return;
    }

    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)){
        return;
    }

    Rect2 rect = {
        this->get_shape<ShapeBase>().get_position(),
        this->get_shape<ShapeBase>().get_size(),
    };

    if(GraphSelection::Ref()->is_in_area(rect)){
        return;
    }

    m_unselect();
}

bool SelectableObject::is_selected(){
    return m_state == State::SELECTED;
}


void SelectableObject::m_emit_event(){
    if(is_selected()
        && was_just_clicked()){
        EventMouseJustClickSelectdObj event;
        event.click_pos = MouseServer::Ref()->get_mouse_world_position();
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);
    }
}