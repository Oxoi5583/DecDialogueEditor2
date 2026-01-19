#pragma once

#include "glm/ext/vector_float2.hpp"
#include "system/obj/abstract/clickable.h"
#include "system/obj/abstract/hoverable.h"
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
    vec2 m_dragging_position_start;

    void m_align_grid();

    bool m_is_align_grid = true;
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

    bool is_align_grid();
    void enable_align_grid();
    void disable_align_grid();
};