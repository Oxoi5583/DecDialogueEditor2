#pragma once

#include <glm/vec2.hpp>
#include "DecToolsBox/abstract/singleton.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "struct/rect2.h"

using namespace glm;

class GraphCamera : public Singleton<GraphCamera>{
private:
    float m_zoom = 1;
    Rect2 m_rect;

    vec2 m_window_size_buffer;
    mat4 m_view;
    mat4 m_origin_view;
    mat4 m_projection;

    void m_job_update_window_size_buffer();
    void m_job_update_view();
    void m_job_update_projection();
public:
    GraphCamera()
    : m_view(mat4(1.0f))
    , m_projection(mat4(1.0f)){};

    void init();
    void update();
    
    void set_target(const vec2& p_target);
    void set_zoom(const float& p_zoom);

    vec2 get_origin() const;
    vec2 get_target() const;
    vec2 get_size() const;
    vec2 get_zoomed_size() const;
    float get_zoom() const;
    Rect2 get_rect() const;

    mat4 get_view() const;
    mat4 get_origin_view() const;
    mat4 get_projection() const;
};
