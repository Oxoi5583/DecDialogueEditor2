#pragma once

#include "glm/ext/vector_float2.hpp"
#include "obj/abstract/clickable.h"
#include "obj/abstract/hoverable.h"
#include "server/object_base.h"

class DragableObject : public ClickableObject {
private:
    enum class State{
        IDLE,
        DRAG,
        PLEACE,
    };

    State m_current_state = State::IDLE;

    void m_update_state();
    void m_handle_action();

    vec2 m_dragging_position_offset;
public:
    DragableObject();
    ~DragableObject();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
    
};