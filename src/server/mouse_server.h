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

    SDL_Cursor* cursors[12] = {nullptr};

    void load_cursor() {
        cursors[0]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
        cursors[1]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
        cursors[2]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
        cursors[3]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
        cursors[4]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
        cursors[5]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
        cursors[6]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
        cursors[7]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
        cursors[8]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
        cursors[9]  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
        cursors[10] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
        cursors[11] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
    }

    void m_emit_event_if_left_just_clicked();
    void m_emit_event_if_left_clicked();
    void m_emit_event_if_left_released();

    void m_event_handle_reset();
    void m_event_handle_hover_event();
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


    void cursor_default();
    void cursor_text();
    void cursor_wait();
    void cursor_crosshair();
    void cursor_pointer();
    void cursor_NS_resize();
    void cursor_EW_resize();
    void cursor_NW_resize();
    void cursor_NE_resize();
    void cursor_move();
    void cursor_not_allowed();
    void cursor_progress();


};