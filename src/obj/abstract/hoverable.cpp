#include "obj/abstract/hoverable.h"
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

    vec2 mouse_pos = MouseServer::Ref()->get_mouse_world_position();
    if(!m_shape->is_point_intersect(mouse_pos)){
        return false;
    }

    EventMouseHoverObj new_event;
    new_event.hovering_pos = mouse_pos;
    new_event.obj_id = get_id();
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
}
void HoverableObject::process(){

}
void HoverableObject::post_process(){

}
void HoverableObject::draw(){

}