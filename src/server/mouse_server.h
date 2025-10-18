#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "glm/ext/vector_float2.hpp"

using namespace glm;

class MouseServer : public Singleton<MouseServer> {
private:
    vec2 m_world_mouse_pos;
    
    bool m_is_just_clicked_on_world();
    bool m_is_clicked_on_world();
    void m_emit_event_if_left_just_clicked_on_world();
    void m_emit_event_if_left_clicked_on_world();
    void m_emit_event_if_left_released();
public:
    void update();
};