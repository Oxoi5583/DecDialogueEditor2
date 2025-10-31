#pragma once

#include "DecToolsBox/abstract/singleton.h"
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
    
    void m_emit_event_if_left_just_clicked();
    void m_emit_event_if_left_clicked();
    void m_emit_event_if_left_released();
public:
    void update();

    vec2 get_mouse_screen_position() const;
    vec2 get_mouse_screen_position_center() const;
    vec2 get_mouse_world_position() const;
    bool is_just_clicked();
    bool is_clicked();
    bool is_just_released();
};