#include "server/mouse_server.h"
#include "engine/input_hub.h"
#include "server/event_server.h"
#include "server/events.h"

void MouseServer::update(){
    m_world_mouse_pos = EngineInputHub::Ref()->get_mouse_world_position();
    m_emit_event_if_left_just_clicked_on_world();
    m_emit_event_if_left_clicked_on_world();
    m_emit_event_if_left_released();
}
bool MouseServer::m_is_just_clicked_on_world(){
    if(EngineInputHub::Ref()->is_mouse_left_button_just_clicked()){
        return true;
    }
    return false;
}
bool MouseServer::m_is_clicked_on_world(){
    if(EngineInputHub::Ref()->is_mouse_left_button_clicked()){
        return true;
    }
    return false;
}
void MouseServer::m_emit_event_if_left_just_clicked_on_world(){
    if(m_is_just_clicked_on_world()){
        EventMouseJustClickedOnWorld event = {m_world_mouse_pos};
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_left_clicked_on_world(){
    if(m_is_clicked_on_world()){
        EventMouseClickedOnWorld event = {m_world_mouse_pos};
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_left_released(){
    if(EngineInputHub::Ref()->is_mouse_left_button_just_released()){
        EventMouseReleased event = {m_world_mouse_pos};
        EventServer::Ref()->emit(event);
    }
}