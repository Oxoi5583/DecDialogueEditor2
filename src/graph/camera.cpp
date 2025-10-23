#include "graph/camera.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "ext/debug/messenger_ext.h"
#include "SDL3/SDL_video.h"
#include "engine/input_hub.h"
#include "engine/input_key.h"
#include "engine/window.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "graph/viewport.h"
#include "imgui/imgui.h"
#include "struct/shape/rect2.h"
#include <cstddef>

void GraphCamera::init(){
    m_rect = Rect2(vec2(0.0f,0.0f), vec2(0.0f,0.0f));
}
void GraphCamera::m_job_update_window_size_buffer(){
    m_viewport_size_buffer = GraphViewport::Ref()->get_viewport_size();
    m_rect.set_size({m_viewport_size_buffer.x,m_viewport_size_buffer.y});
}
void GraphCamera::m_job_update_view(){
    m_view = glm::translate(glm::mat4(1.0f), glm::vec3(-m_rect.get_center(), 0.0f));
    m_view = glm::translate(m_view, glm::vec3(GraphViewport::Ref()->get_viewport_rect().get_center(), 0.0f));
    m_view = glm::translate(m_view, glm::vec3(-EngineWindow::Ref()->get_window_size() / 2.0f, 0.0f));
    m_origin_view = glm::translate(glm::mat4(1.0f), glm::vec3(-m_rect.get_left_top(), 0.0f));
}
void GraphCamera::m_job_update_projection(){
    vec2 zoomed_window_size = EngineWindow::Ref()->get_window_size() / m_zoom;

    m_projection = glm::ortho(
        -zoomed_window_size.x * 0.5f, zoomed_window_size.x * 0.5f,
        zoomed_window_size.y * 0.5f, -zoomed_window_size.y * 0.5f,
        -1.0f, 1.0f
    );
}
void GraphCamera::m_job_update_control(){
    vec2 motion = vec2(0.0f,0.0f);

    const double speed = 5;
    if(EngineInputHub::Ref()->keyboard_is_down(K_UP)){
        motion.y -= speed;
    }
    if(EngineInputHub::Ref()->keyboard_is_down(K_DOWN)){
        motion.y += speed;
    }
    if(EngineInputHub::Ref()->keyboard_is_down(K_LEFT)){
        motion.x -= speed;
    }
    if(EngineInputHub::Ref()->keyboard_is_down(K_RIGHT)){
        motion.x += speed;
    }

    vec2 old_target = this->get_target();
    vec2 new_target = old_target + motion;

    this->set_target(new_target);
}
void GraphCamera::m_job_draw_border(){
    std::vector<vec2> points = m_rect.get_points();
    EngineRenderer::Ref()->draw_line(points[0], points[1], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    EngineRenderer::Ref()->draw_line(points[1], points[2], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    EngineRenderer::Ref()->draw_line(points[2], points[3], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    EngineRenderer::Ref()->draw_line(points[3], points[0], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    
}
void GraphCamera::update(){
    m_job_update_control();
    m_job_update_window_size_buffer();
    m_job_update_view();
    m_job_update_projection();
    m_job_draw_border();
}
void GraphCamera::set_target(const vec2& p_target){
    m_rect.set_center(p_target);
}
void GraphCamera::set_zoom(const float& p_zoom){
    m_zoom = p_zoom;
}

vec2 GraphCamera::get_origin() const{
    return m_rect.get_left_top();
}
vec2 GraphCamera::get_target() const{
    return m_rect.get_center();
}
vec2 GraphCamera::get_size() const{
    return m_rect.get_size();
}
vec2 GraphCamera::get_zoomed_size() const{
    return vec4(m_rect.get_size() / m_zoom, 0.0f, 1.0f);
}
float GraphCamera::get_zoom() const{
    return m_zoom;
}
Rect2 GraphCamera::get_zoomed_rect() const{
    Rect2 ret = m_rect;
    ret.set_size(get_zoomed_size());
    return ret;
}
Rect2 GraphCamera::get_rect() const{
    return m_rect;
}


mat4 GraphCamera::get_view() const{
    return m_view;
}
mat4 GraphCamera::get_origin_view() const{
    return m_origin_view;
}
mat4 GraphCamera::get_projection() const{
    return m_projection;
}


vec2 GraphCamera::world_to_viewport(vec2 p_pos){
    mat4 transform = mat4(1.0f);
    transform = glm::translate(transform, vec3(get_zoomed_rect().get_left_top(),0.0f));
    return inverse(transform) * vec4(p_pos, 0.0f, 1.0f);
}
vec2 GraphCamera::viewport_to_world(vec2 p_pos){
    Rect2 zoomed_rect = get_zoomed_rect();
    vec2 zoomed_size = zoomed_rect.get_size();

    Rect2 viewport_rect2 = GraphViewport::Ref()->get_viewport_rect();
    vec2 viewport_size = viewport_rect2.get_size();
    vec2 pos_ratio = {
        std::clamp(p_pos.x / viewport_size.x, 0.0f, 1.0f),
        std::clamp(p_pos.y / viewport_size.y, 0.0f, 1.0f),
    };


    vec2 converted_pos = zoomed_size * pos_ratio;

    mat4 transform = mat4(1.0f);
    transform = glm::translate(transform, vec3(get_zoomed_rect().get_left_top(),0.0f));
    vec2 world_pos = transform * vec4(converted_pos, 0.0f, 1.0f);
    return world_pos;
}
