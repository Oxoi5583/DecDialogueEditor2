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
    m_is_mouse_in_window = EngineInputHub::Ref()->is_mouse_in_window();
    
    m_emit_event_if_left_just_clicked();
    m_emit_event_if_left_clicked();
    m_emit_event_if_left_released();

    m_event_handle_reset();
    m_event_handle_resize_event();
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

void MouseServer::m_set_cursor(int p_index){
    SDL_SetCursor(cursors[p_index]);
}

void MouseServer::m_event_handle_reset(){
    m_resizer_event = EventServer::Ref()->poll<EventMouseOnResizer>();
    m_obj_hover_event = EventServer::Ref()->poll<EventMouseHoverObj>();
    if(m_resizer_event.empty() && m_obj_hover_event.empty()){
        this->cursor_default();
    }
}

void MouseServer::m_event_handle_resize_event(){
    for(EventMouseOnResizer event : m_resizer_event){
        switch (event.dir) {
            case EventDirection::UP:
                this->cursor_N_resize();
                break;
            case EventDirection::DOWN:
                this->cursor_S_resize();
                break;
            case EventDirection::LEFT:
                this->cursor_W_resize();
                break;
            case EventDirection::RIGHT:
                this->cursor_E_resize();
                break;
            case EventDirection::UP_LEFT:
                this->cursor_NW_resize();
                break;
            case EventDirection::DOWN_LEFT:
                this->cursor_SW_resize();
                break;
            case EventDirection::UP_RIGHT:
                this->cursor_NE_resize();
                break;
            case EventDirection::DOWN_RIGHT:
                this->cursor_SE_resize();
                break;
        }
    }
}
void MouseServer::m_event_handle_hover_event(){
    for(EventMouseHoverObj event : m_obj_hover_event){
        if(ObjectServer::Ref()->is_id_valid(event.obj_id)){
            ObjectBase* ptr = ObjectServer::Ref()->get_instance<HoverableObject>(event.obj_id);
            ClickableObject* c_ptr = dynamic_cast<ClickableObject*>(ptr);

            if(!c_ptr){
                continue;
            }

            if(!c_ptr->is_changing_cursor()){
                this->cursor_default();
            }else{
                this->cursor_pointer();
            }
            
        }
    }
}

vec2 MouseServer::get_mouse_screen_position() const{ return m_screen_mouse_pos; }
vec2 MouseServer::get_mouse_screen_position_center() const{ return m_screen_mouse_pos_center; }
vec2 MouseServer::get_mouse_world_position() const{ return m_world_mouse_pos; }

bool MouseServer::is_just_clicked(){ return m_is_just_clicked; }
bool MouseServer::is_clicked(){ return m_is_clicked; }
bool MouseServer::is_just_released(){ return m_is_just_released; }
bool MouseServer::is_mouse_in_window(){ return m_is_mouse_in_window; }

void MouseServer::cursor_default()      { m_set_cursor(SDL_SYSTEM_CURSOR_DEFAULT); }
void MouseServer::cursor_text()         { m_set_cursor(SDL_SYSTEM_CURSOR_TEXT); }
void MouseServer::cursor_wait()         { m_set_cursor(SDL_SYSTEM_CURSOR_WAIT); }
void MouseServer::cursor_crosshair()    { m_set_cursor(SDL_SYSTEM_CURSOR_CROSSHAIR); }
void MouseServer::cursor_progress()     { m_set_cursor(SDL_SYSTEM_CURSOR_PROGRESS); }
void MouseServer::cursor_pointer()      { m_set_cursor(SDL_SYSTEM_CURSOR_POINTER); }
void MouseServer::cursor_move()         { m_set_cursor(SDL_SYSTEM_CURSOR_MOVE); }
void MouseServer::cursor_not_allowed()  { m_set_cursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED); }

void MouseServer::cursor_NS_resize()    { m_set_cursor(SDL_SYSTEM_CURSOR_NS_RESIZE); }
void MouseServer::cursor_EW_resize()    { m_set_cursor(SDL_SYSTEM_CURSOR_EW_RESIZE); }
void MouseServer::cursor_NWSE_resize()  { m_set_cursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE); }
void MouseServer::cursor_NESW_resize()  { m_set_cursor(SDL_SYSTEM_CURSOR_NESW_RESIZE); }
void MouseServer::cursor_NW_resize()    { m_set_cursor(SDL_SYSTEM_CURSOR_NW_RESIZE); }
void MouseServer::cursor_N_resize()     { m_set_cursor(SDL_SYSTEM_CURSOR_N_RESIZE); }
void MouseServer::cursor_NE_resize()    { m_set_cursor(SDL_SYSTEM_CURSOR_NE_RESIZE); }
void MouseServer::cursor_E_resize()     { m_set_cursor(SDL_SYSTEM_CURSOR_E_RESIZE); }
void MouseServer::cursor_SE_resize()    { m_set_cursor(SDL_SYSTEM_CURSOR_SE_RESIZE); }
void MouseServer::cursor_S_resize()     { m_set_cursor(SDL_SYSTEM_CURSOR_S_RESIZE); }
void MouseServer::cursor_SW_resize()    { m_set_cursor(SDL_SYSTEM_CURSOR_SW_RESIZE); }
void MouseServer::cursor_W_resize()     { m_set_cursor(SDL_SYSTEM_CURSOR_W_RESIZE); }
