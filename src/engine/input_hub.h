#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "SDL3/SDL_events.h"
#include <glm/glm.hpp>
#include <set>
#include <unordered_set>
#include "engine/input_key.h"

using namespace glm;

class EngineInputHub : public Singleton<EngineInputHub>{
private:
    vec2 m_mouse_last_position;
    vec2 m_mouse_motion;
    vec2 m_mouse_position;
    vec2 m_mouse_wheel;
    bool m_mouse_left_button_just_clicked = false;
    bool m_mouse_right_button_just_clicked = false;
    bool m_mouse_left_button_just_released = false;
    bool m_mouse_right_button_just_released = false;
    bool m_mouse_left_button_clicked = false;
    bool m_mouse_right_button_clicked = false;

    bool m_is_mouse_in_window = false;

    bool m_is_close_requested = false;
    SDL_WindowID m_close_window_id;
    
    std::unordered_set<EngineKeycode> m_keyboard_down_buffer;
    std::unordered_set<EngineKeycode> m_keyboard_just_down_buffer;
    std::unordered_set<EngineKeycode> m_keyboard_just_up_buffer;
    void m_redirect_keyboard_down_buffer(SDL_Keycode p_key);
    void m_redirect_keyboard_up_buffer(SDL_Keycode p_key);
    void m_store_keyboard_down_buffer(SDL_Keycode p_key);
    void m_store_keyboard_up_buffer(SDL_Keycode p_key);
public:
    vec2 get_mouse_motion();
    vec2 get_mouse_position();
    vec2 get_mouse_world_position();
    vec2 get_mouse_wheel();
    bool is_mouse_left_button_just_clicked();
    bool is_mouse_right_button_just_clicked();
    bool is_mouse_left_button_just_released();
    bool is_mouse_right_button_just_released();
    bool is_mouse_left_button_clicked();
    bool is_mouse_right_button_clicked();

    bool is_mouse_in_window();

    bool is_close_requested();
    Uint32 get_close_window_id();

    bool keyboard_is_just_down(EngineKeycode p_key);
    bool keyboard_is_just_up(EngineKeycode p_key);
    bool keyboard_is_down(EngineKeycode p_key);

    void polling_sdl_event();
};

