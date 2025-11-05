#include "server/mouse_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_mouse.h"
#include "engine/input_hub.h"
#include "engine/window.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/abstract/clickable.h"
#include "obj/abstract/hoverable.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"

MouseServer::MouseServer(){
    load_cursor();
}
MouseServer::~MouseServer(){
    for(SDL_Cursor* cursor : cursors){
        if (cursor){
            SDL_DestroyCursor(cursor);
        }
    }
}

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

    m_event_handle_reset();
    m_event_handle_hover_event();
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

void MouseServer::m_event_handle_reset(){
    this->cursor_default();
}

void MouseServer::m_event_handle_hover_event(){
    auto events = EventServer::Ref()->poll<EventMouseHoverObj>();
    for(EventMouseHoverObj event : events){
        if(ObjectServer::Ref()->is_id_valid(event.obj_id)){
            ObjectBase* ptr = ObjectServer::Ref()->get_instance<HoverableObject>(event.obj_id);
            ClickableObject* c_ptr = dynamic_cast<ClickableObject*>(ptr);

            if(!c_ptr){
                continue;
            }

            if(!c_ptr->is_changing_cursor()){
                continue;
            }
            
            this->cursor_pointer();
        }
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


void MouseServer::cursor_default() { SDL_SetCursor(cursors[0]); }
void MouseServer::cursor_text() { SDL_SetCursor(cursors[1]); }
void MouseServer::cursor_wait() { SDL_SetCursor(cursors[2]); }
void MouseServer::cursor_crosshair() { SDL_SetCursor(cursors[3]); }
void MouseServer::cursor_pointer() { SDL_SetCursor(cursors[4]); }
void MouseServer::cursor_NS_resize() { SDL_SetCursor(cursors[5]); }
void MouseServer::cursor_EW_resize() { SDL_SetCursor(cursors[6]); }
void MouseServer::cursor_NW_resize() { SDL_SetCursor(cursors[7]); }
void MouseServer::cursor_NE_resize() { SDL_SetCursor(cursors[8]); }
void MouseServer::cursor_move() { SDL_SetCursor(cursors[9]); }
void MouseServer::cursor_not_allowed() { SDL_SetCursor(cursors[10]); }
void MouseServer::cursor_progress() { SDL_SetCursor(cursors[11]); }

