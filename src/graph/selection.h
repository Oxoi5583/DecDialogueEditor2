#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"

using namespace glm;

class GraphSelection : public Singleton<GraphSelection>{
private:
    enum State{
        IDLE,
        DRAGGING,
    };

    State m_state = State::IDLE;
    vec2 m_start_dragging_pos;
    vec2 m_end_dragging_pos;

    Rect2 m_selection_area;

    void m_update_state();

    void m_update_state_TO_IDLE();
    void m_update_state_TO_DRAGGING();

    void m_process();
public:
    void init();
    void pre_update();
    void post_update();
    void draw();

    bool is_selecting();
    bool is_in_area(Rect2& p_rect);
};
