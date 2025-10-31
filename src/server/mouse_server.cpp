#include "server/mouse_server.h"
#include "engine/input_hub.h"
#include "engine/window.h"
#include "glm/ext/vector_float2.hpp"
#include "server/event_server.h"
#include "server/events.h"

void MouseServer::update(){
    m_world_mouse_pos = EngineInputHub::Ref()->get_mouse_world_position();
    m_screen_mouse_pos = EngineInputHub::Ref()->get_mouse_position();
    m_screen_mouse_pos_center = m_screen_mouse_pos - (EngineWindow::Ref()->get_window_size() / 2.0f);
    m_is_just_clicked = EngineInputHub::Ref()->is_mouse_left_button_just_clicked();
    m_is_clicked = EngineInputHub::Ref()->is_mouse_left_button_clicked();
    m_is_just_released = EngineInputHub::Ref()->is_mouse_left_button_just_released();
    m_emit_event_if_left_just_clicked();
    m_emit_event_if_left_clicked();
    m_emit_event_if_left_released();
}
void MouseServer::m_emit_event_if_left_just_clicked(){
    if(m_is_just_clicked){
        EventMouseJustClicked event;
        event.click_pos = m_world_mouse_pos;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_left_clicked(){
    if(m_is_clicked){
        EventMouseClicked event;
        event.click_pos = m_world_mouse_pos;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_left_released(){
    if(m_is_just_released){
        EventMouseReleased event;
        event.click_pos = m_screen_mouse_pos;
        EventServer::Ref()->emit(event);
    }
}

vec2 MouseServer::get_mouse_screen_position() const{
    return m_screen_mouse_pos;
}
vec2 MouseServer::get_mouse_screen_position_center() const{
    return m_screen_mouse_pos_center;
}

vec2 MouseServer::get_mouse_world_position() const{
    return m_world_mouse_pos;
}


bool MouseServer::is_just_clicked(){
    return m_is_just_clicked;
}
bool MouseServer::is_clicked(){
    return m_is_clicked;
}
bool MouseServer::is_just_released(){
    return m_is_just_released;
}
