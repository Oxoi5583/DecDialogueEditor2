#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "SDL3/SDL_events.h"
#include <glm/glm.hpp>

using namespace glm;

class EngineEventHub : public Singleton<EngineEventHub>{
private:
    SDL_Event m_sdl_event;

    vec2 m_mouse_position;
    vec2 m_mouse_wheel;
    bool m_mouse_left_button_just_clicked = false;
    bool m_mouse_right_button_just_clicked = false;
    bool m_mouse_left_button_just_released = false;
    bool m_mouse_right_button_just_released = false;
    bool m_mouse_left_button_clicked = false;
    bool m_mouse_right_button_clicked = false;

    bool m_is_close_requested = false;
    SDL_WindowID m_close_window_id;

public:
    vec2 get_mouse_position();
    vec2 get_mouse_world_position();
    vec2 get_mouse_wheel();
    bool is_mouse_left_button_just_clicked();
    bool is_mouse_right_button_just_clicked();
    bool is_mouse_left_button_just_released();
    bool is_mouse_right_button_just_released();
    bool is_mouse_left_button_clicked();
    bool is_mouse_right_button_clicked();

    bool is_close_requested();
    Uint32 get_close_window_id();

    void polling();
};

