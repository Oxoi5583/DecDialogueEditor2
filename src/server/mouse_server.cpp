#include "server/mouse_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_mouse.h"
#include "engine/window.h"
#include "engine/input_hub.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_server.h"
#include "obj/abstract/hoverable.h"
#include "obj/abstract/clickable.h"

MouseServer::MouseServer() {
    load_cursor();
}

MouseServer::~MouseServer() {
    for (SDL_Cursor* cursor : cursors) {
        if (cursor) SDL_DestroyCursor(cursor);
    }
}
void MouseServer::load_cursor() {
    cursors.resize(SDL_SYSTEM_CURSOR_COUNT);
    cursors[SDL_SYSTEM_CURSOR_DEFAULT]      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    cursors[SDL_SYSTEM_CURSOR_TEXT]         = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
    cursors[SDL_SYSTEM_CURSOR_WAIT]         = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
    cursors[SDL_SYSTEM_CURSOR_CROSSHAIR]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    cursors[SDL_SYSTEM_CURSOR_PROGRESS]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
    cursors[SDL_SYSTEM_CURSOR_NWSE_RESIZE]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_NESW_RESIZE]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_EW_RESIZE]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_NS_RESIZE]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_MOVE]         = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
    cursors[SDL_SYSTEM_CURSOR_NOT_ALLOWED]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
    cursors[SDL_SYSTEM_CURSOR_POINTER]      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    cursors[SDL_SYSTEM_CURSOR_NW_RESIZE]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NW_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_N_RESIZE]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_N_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_NE_RESIZE]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NE_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_E_RESIZE]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_E_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_SE_RESIZE]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SE_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_S_RESIZE]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_S_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_SW_RESIZE]    = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SW_RESIZE);
    cursors[SDL_SYSTEM_CURSOR_W_RESIZE]     = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_W_RESIZE);
};

void MouseServer::update() {
    auto hub = EngineInputHub::Ref();

    m_world_mouse_pos = hub->get_mouse_world_position();
    m_screen_mouse_pos = hub->get_mouse_position();
    m_screen_mouse_pos_center = m_screen_mouse_pos - (EngineWindow::Ref()->get_window_size() / 2.0f);
    m_is_mouse_in_window = hub->is_mouse_in_window();

    m_is_left_just_clicked = hub->is_mouse_left_button_just_clicked();
    m_is_left_clicked = hub->is_mouse_left_button_clicked();
    m_is_left_just_released = hub->is_mouse_left_button_just_released();

    m_is_right_just_clicked = hub->is_mouse_right_button_just_clicked();
    m_is_right_clicked = hub->is_mouse_right_button_clicked();
    m_is_right_just_released = hub->is_mouse_right_button_just_released();

    m_is_middle_just_clicked = hub->is_mouse_middle_button_just_clicked();
    m_is_middle_clicked = hub->is_mouse_middle_button_clicked();
    m_is_middle_just_released = hub->is_mouse_middle_button_just_released();

    m_emit_event_if_left_just_clicked();
    m_emit_event_if_left_clicked();
    m_emit_event_if_left_released();

    m_emit_event_if_right_just_clicked();
    m_emit_event_if_right_clicked();
    m_emit_event_if_right_released();

    m_emit_event_if_middle_just_clicked();
    m_emit_event_if_middle_clicked();
    m_emit_event_if_middle_released();

    m_event_handle_reset();
    m_event_handle_resize_event();
    m_event_handle_hover_event();
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
void MouseServer::m_emit_event_if_left_just_clicked() {
    if (m_is_left_just_clicked) {
        EventMouseJustClicked event;
        event.click_pos = m_world_mouse_pos;
        event.button = (int)MouseButton::LEFT;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_left_clicked() {
    if (m_is_left_clicked) {
        EventMouseClicked event;
        event.click_pos = m_world_mouse_pos;
        event.button = (int)MouseButton::LEFT;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_left_released() {
    if (m_is_left_just_released) {
        EventMouseReleased event;
        event.click_pos = m_screen_mouse_pos;
        event.button = (int)MouseButton::LEFT;
        EventServer::Ref()->emit(event);
    }
}

void MouseServer::m_emit_event_if_right_just_clicked() {
    if (m_is_right_just_clicked) {
        EventMouseJustClicked event;
        event.click_pos = m_world_mouse_pos;
        event.button = (int)MouseButton::RIGHT;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_right_clicked() {
    if (m_is_right_clicked) {
        EventMouseClicked event;
        event.click_pos = m_world_mouse_pos;
        event.button = (int)MouseButton::RIGHT;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_right_released() {
    if (m_is_right_just_released) {
        EventMouseReleased event;
        event.click_pos = m_screen_mouse_pos;
        event.button = (int)MouseButton::RIGHT;
        EventServer::Ref()->emit(event);
    }
}

void MouseServer::m_emit_event_if_middle_just_clicked() {
    if (m_is_middle_just_clicked) {
        EventMouseJustClicked event;
        event.click_pos = m_world_mouse_pos;
        event.button = (int)MouseButton::MIDDLE;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_middle_clicked() {
    if (m_is_middle_clicked) {
        EventMouseClicked event;
        event.click_pos = m_world_mouse_pos;
        event.button = (int)MouseButton::MIDDLE;
        EventServer::Ref()->emit(event);
    }
}
void MouseServer::m_emit_event_if_middle_released() {
    if (m_is_middle_just_released) {
        EventMouseReleased event;
        event.click_pos = m_screen_mouse_pos;
        event.button = (int)MouseButton::MIDDLE;
        EventServer::Ref()->emit(event);
    }
}

vec2 MouseServer::get_mouse_screen_position() const { return m_screen_mouse_pos; }
vec2 MouseServer::get_mouse_screen_position_center() const { return m_screen_mouse_pos_center; }
vec2 MouseServer::get_mouse_world_position() const { return m_world_mouse_pos; }

bool MouseServer::is_mouse_in_window() { return m_is_mouse_in_window; }

bool MouseServer::is_just_clicked(MouseButton p_button) {
    switch (p_button) {
        case MouseButton::LEFT: return m_is_left_just_clicked;
        case MouseButton::RIGHT: return m_is_right_just_clicked;
        case MouseButton::MIDDLE: return m_is_middle_just_clicked;
    }
    return false;
}
bool MouseServer::is_clicked(MouseButton p_button) {
    switch (p_button) {
        case MouseButton::LEFT: return m_is_left_clicked;
        case MouseButton::RIGHT: return m_is_right_clicked;
        case MouseButton::MIDDLE: return m_is_middle_clicked;
    }
    return false;
}
bool MouseServer::is_just_released(MouseButton p_button) {
    switch (p_button) {
        case MouseButton::LEFT: return m_is_left_just_released;
        case MouseButton::RIGHT: return m_is_right_just_released;
        case MouseButton::MIDDLE: return m_is_middle_just_released;
    }
    return false;
}

void MouseServer::m_set_cursor(int p_index) {
    if (p_index >= 0 && p_index < (int)cursors.size())
        SDL_SetCursor(cursors[p_index]);
}

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
