#pragma once

#include <glm/vec2.hpp>
#include "DecToolsBox/abstract/singleton.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"

using namespace glm;

class GraphCamera : public Singleton<GraphCamera>{
private:
    float m_zoom = 1;
    Rect2 m_rect;

    vec2 m_viewport_size_buffer;
    mat4 m_view;
    mat4 m_origin_view;
    mat4 m_projection;

    bool m_is_dragging = false;
    vec2 m_dragging_start_pos;
    vec2 m_dragging_start_target;

    vec2 m_left_top_buffer;

    void m_job_update_window_size_buffer();
    void m_job_update_view();
    void m_job_update_projection();
    void m_job_update_is_dragging();
    void m_job_update_control();
    void m_job_draw_border();
public:
    GraphCamera()
    : m_view(mat4(1.0f))
    , m_projection(mat4(1.0f)){};

    void init();
    void update();
    
    void set_target(const vec2& p_target);
    void set_zoom(const float& p_zoom);
    
    void refresh_left_top_buffer();
    vec2 get_left_top_buffer() const;
    void go_to_left_top_buffer();

    vec2 get_origin() const;
    vec2 get_target() const;
    vec2 get_size() const;
    vec2 get_zoomed_size() const;
    float get_zoom() const;
    Rect2 get_rect() const;
    Rect2 get_zoomed_rect() const;

    mat4 get_view() const;
    mat4 get_origin_view() const;
    mat4 get_projection() const;

    vec2 world_to_viewport(vec2 p_pos);
    vec2 viewport_to_world(vec2 p_pos);

    bool is_rect_on_camera(Rect2 p_rect);
};
