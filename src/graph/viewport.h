#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"
#include "ext/debug/messenger_ext.h"

using namespace glm;

class GraphViewport : public Singleton<GraphViewport>{
private:
    vec2 m_window_size;
    Rect2 m_viewport_rect;
    Rect2 m_viewport_rect_buffer;

    void m_job_update_control();
public:
    void init();
    void update();

    Rect2 get_viewport_rect();

    void set_viewport_position(vec2 p_pos);
    vec2 get_viewport_position();
    void set_viewport_center(vec2 p_pos);
    vec2 get_viewport_center();

    void set_viewport_size(vec2 p_size);
    vec2 get_viewport_size();

    vec2 get_viewport_start();
    vec2 get_viewport_end();
    void set_viewport(double p_start_x, double p_end_x, double p_start_y, double p_end_y);

    void set_viewport_up_border(double p_y);
    void set_viewport_down_border(double p_y);
    void set_viewport_left_border(double p_x);
    void set_viewport_right_border(double p_x);
    double get_viewport_up_border();
    double get_viewport_down_border();
    double get_viewport_left_border();
    double get_viewport_right_border();

    vec2 screen_to_viewport(vec2 p_pos);
    vec2 viewport_to_screen(vec2 p_pos);
};