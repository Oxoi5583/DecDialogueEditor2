#pragma once

#include "glm/ext/vector_float2.hpp"
#include "obj/abstract/clickable.h"
#include "obj/abstract/hoverable.h"
#include "server/object_base.h"

class DragableObject : public ClickableObject {
private:
    enum class State{
        IDLE,
        READY,
        RESTORE_IDLE,
        DRAG,
        PLACE,
    };

    State m_current_state = State::IDLE;

    void m_update_state();
    void m_handle_action();

    void m_emit_event();

    vec2 m_ready_mouse_pos;
    vec2 m_dragging_position_offset;
public:
    DragableObject();
    ~DragableObject();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
    
    bool is_dragging();
    bool is_drag_ready();
    bool is_restore_to_idle();
    bool is_placed();

    void drag_ready();
    void drag();
    void place();
};