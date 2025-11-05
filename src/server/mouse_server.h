#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
#include "glm/ext/vector_float2.hpp"

using namespace glm;

class MouseServer : public Singleton<MouseServer> {
private:
    vec2 m_world_mouse_pos;
    vec2 m_screen_mouse_pos;
    vec2 m_screen_mouse_pos_center;
    bool m_is_just_clicked;
    bool m_is_clicked;
    bool m_is_just_released;
    bool m_is_mouse_in_window = false;

    SDL_Cursor* cursors[SDL_SYSTEM_CURSOR_COUNT] = {nullptr};

    void load_cursor() {
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
    }

    void m_emit_event_if_left_just_clicked();
    void m_emit_event_if_left_clicked();
    void m_emit_event_if_left_released();

    void m_event_handle_reset();
    void m_event_handle_resize_event();
    void m_event_handle_hover_event();

    void m_set_cursor(int p_index);
public:
    MouseServer();
    ~MouseServer();

    void update();

    vec2 get_mouse_screen_position() const;
    vec2 get_mouse_screen_position_center() const;
    vec2 get_mouse_world_position() const;
    bool is_just_clicked();
    bool is_clicked();
    bool is_just_released();
    bool is_mouse_in_window();

    void cursor_default();
    void cursor_text();
    void cursor_wait();
    void cursor_crosshair();
    void cursor_progress();
    void cursor_pointer();
    void cursor_move();
    void cursor_not_allowed();

    void cursor_NS_resize();
    void cursor_EW_resize();
    void cursor_NWSE_resize();
    void cursor_NESW_resize();
    void cursor_NW_resize();
    void cursor_N_resize();
    void cursor_NE_resize();
    void cursor_E_resize();
    void cursor_SE_resize();
    void cursor_S_resize();
    void cursor_SW_resize();
    void cursor_W_resize();
};
