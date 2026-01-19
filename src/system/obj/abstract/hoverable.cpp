#include "system/obj/abstract/hoverable.h"
#include "DecToolsBox/debug/messenger.h"
#include "glm/ext/vector_float2.hpp"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "struct/shape/base.h"
#include <memory>


HoverableObject::HoverableObject(){
    BIND_CLASS(HoverableObject);
}
HoverableObject::~HoverableObject(){

}

bool HoverableObject::m_check_hovering(){
    EventMouseHoverObj event = EventServer::Ref()->poll_first<EventMouseHoverObj>();
    if(event.is_event_occurred && event.obj_id != get_id()){
        return false;
    }

    vec2 mouse_pos = get_mouse_pos();
    
    if(!this->is_point_intersect(mouse_pos)){
        return false;
    }

    EventMouseHoverObj new_event;
    new_event.hovering_pos = mouse_pos;
    new_event.obj_id = get_id();
    new_event.is_pointer_cursor = m_changed_cursor;
    EventServer::Ref()->emit(new_event);
    return true;
}


bool HoverableObject::was_hovered(){
    return m_was_hovered;
}

void HoverableObject::ready(){
}
void HoverableObject::pre_process(){
    m_was_hovered = m_check_hovering();
    if(this->was_hovered()){
        EventMouseHoverObjLastFrame new_event2;
        new_event2.hovering_pos = get_mouse_pos();
        new_event2.obj_id = get_id();
        new_event2.is_pointer_cursor = m_changed_cursor;
        EventServer::Ref()->emit(new_event2);
    }
}
void HoverableObject::process(){

}
void HoverableObject::post_process(){

}
void HoverableObject::draw(){

}

bool HoverableObject::is_changing_cursor(){
    return m_changed_cursor;
}
void HoverableObject::disable_cursor_change(){
    m_changed_cursor = false;
}
void HoverableObject::enable_cursor_change(){
    m_changed_cursor = true;
}


HoverableObject::Type HoverableObject::get_hovering_type(){
    return m_type;
}
void HoverableObject::set_hovering_type(HoverableObject::Type p_type){
    m_type = p_type;
}

vec2 HoverableObject::get_mouse_pos(){
    switch (m_type) {
        case Type::SCREEN:
            return MouseServer::Ref()->get_mouse_screen_position_center();
            break;
        case Type::WORLD:
            return MouseServer::Ref()->get_mouse_world_position();
            break;
    }
}