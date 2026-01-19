#include "system/obj/abstract/selectable.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/input_hub.h"
#include "engine/input_key.h"
#include "system/graph/selection.h"
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

void SelectableObject::select(){
    m_state = State::READY;
}
void SelectableObject::unselect(){
    m_state = State::RESTORE;
}

void SelectableObject::m_update_state(){
    if(EventServer::Ref()->has<EventLockedAll>()){
        return;
    }

    switch (m_state) {
        case IDLE:{
            m_select_if_clicked();
            m_select_if_in_select_area();
            break;
        }
        case READY:{
            m_state = State::SELECTED;
            break;
        }
        case SELECTED:{
            m_unselect_if_lshift_clicked();
            m_unselect_if_other_clicked();
            m_unselect_if_world_clicked();
            m_unselect_if_not_in_select_area();
            break;
        }
        case RESTORE:{
            m_state = State::IDLE;
            break;
        }
    }
}


void SelectableObject::m_select_if_clicked(){
    if(!this->is_on_camera()){
        return;
    }

    if(this->was_just_clicked()){
        select();
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

    select();
}

void SelectableObject::m_unselect_if_lshift_clicked(){
    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)
        && this->was_just_clicked()){
        unselect();
    }
}
void SelectableObject::m_unselect_if_other_clicked(){
    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)){
        return;
    }

    if(GraphSelection::Ref()->is_group_dragging()){
        return;
    }

    if(!EventServer::Ref()->has<EventMouseJustClickObj>()){
        return;
    }

    if(EventServer::Ref()->poll_first<EventMouseJustClickObj>().obj_id == this->get_id()){
        return;
    }

    if(!MouseServer::Ref()->is_mouse_in_viewport()){
        return;
    }

    if(EventServer::Ref()->has<EventMouseJustClickSelectedObj>()){
        return;
    }

    unselect();
}
void SelectableObject::m_unselect_if_world_clicked(){
    if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)){
        return;
    }

    if(EventServer::Ref()->has<EventMouseJustClickedOnWorld>()){
        unselect();
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

    unselect();
}

bool SelectableObject::is_selected(){
    return m_state == State::SELECTED;
}

void SelectableObject::m_emit_event(){
    if(is_selected() && was_just_clicked()){
        EventMouseJustClickSelectedObj event;
        event.click_pos = MouseServer::Ref()->get_mouse_world_position();
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);
    }
    if(is_selected()){
        EventMouseSelectedObj event;
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);
    }
    if(is_drag_ready() && is_selected()){
        GraphSelection::Ref()->store_selection_buffer();
    }
    if(was_hovered() && is_selected()){
        EventSelectedObjHovering event;
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);
    }
    if(is_dragging() && is_selected()){
        EventSelectedObjDragging event;
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);

        GraphSelection::Ref()->drag_all_selection();
    }
    if(is_placed() && is_selected()){
        GraphSelection::Ref()->release_selection_buffer();
        EventSelectedObjPlaced event;
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);
    }
    if(is_restore_to_idle() && is_selected()){
        GraphSelection::Ref()->release_selection_buffer();
    }
}