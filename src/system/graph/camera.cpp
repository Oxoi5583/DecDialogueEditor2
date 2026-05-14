#include "system/graph/camera.h"
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
#include "glm/geometric.hpp"
#include "system/graph/viewport.h"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "struct/shape/rect2.h"
#include <cstddef>
#include <server/physics_server.h>
#include <server/project_server.h>

#include "ext/debug/messenger_ext.h"

void GraphCamera::init(){
    m_rect = Rect2(vec2(0.0f,0.0f), vec2(0.0f,0.0f));
    refresh_left_top_buffer();
}
void GraphCamera::m_job_update_window_size_buffer(){
    m_viewport_size_buffer = GraphViewport::Ref()->get_viewport_size();

    vec2 lt = m_rect.get_left_top();
    vec2 rd = lt + m_viewport_size_buffer;
    m_rect.set_AABB(lt,rd);

    Rect2 camera_rect = this->get_zoomed_rect();
    PhysicsServer::Ref()->set_instance(m_shape_id, {camera_rect.get_left_top(), camera_rect.get_right_down()});
    
}
void GraphCamera::m_job_update_view(){
    vec2 pivot = GraphViewport::Ref()->get_viewport_rect().get_center();

    m_view = glm::mat4(1.0f);
    m_view = glm::translate(m_view, glm::vec3(pivot , 0.0f));
    m_view = glm::scale(m_view, {m_zoom, m_zoom, 1.0f});
    //m_view = glm::translate(m_view, glm::vec3(-pivot , 0.0f));
    //m_view = glm::translate(m_view, glm::vec3(GraphViewport::Ref()->get_viewport_rect().get_center(), 0.0f));

    m_view = glm::translate(m_view, glm::vec3(-m_rect.get_center(), 0.0f));

    m_origin_view = glm::translate(glm::mat4(1.0f), glm::vec3(-m_rect.get_left_top(), 0.0f));
}
void GraphCamera::m_job_update_projection(){
    vec2 window_size = EngineWindow::Ref()->get_window_size();

    m_projection = glm::ortho(
        0.0f, window_size.x,
        window_size.y, 0.0f,
        -1.0f, 1.0f
    );
}
void GraphCamera::m_job_update_is_dragging(){
    if(m_is_dragging){
        if(MouseServer::Ref()->is_just_released(MouseButton::MIDDLE)){
            m_is_dragging = false;
        }
        return;
    }

    if(EventServer::Ref()->has<EventMouseJustClickedOnWorld>()){
        auto event = EventServer::Ref()->poll_first<EventMouseJustClickedOnWorld>();
        if(event.button != (int)MouseButton::MIDDLE){
            return;     
        }

        m_dragging_start_pos = event.screen_pos;
        m_dragging_start_target = this->get_target();
        m_is_dragging = true;
    }
}
void GraphCamera::m_job_update_control_motion(){
    if(!m_is_dragging){
        return;
    }

    vec2 current_mouse_pos = MouseServer::Ref()->get_mouse_screen_position();
    vec2 motion = current_mouse_pos - m_dragging_start_pos;
    vec2 new_target = m_dragging_start_target - motion;

    this->set_target(new_target);
}
void GraphCamera::m_job_update_control_zoom(){
    if(m_is_dragging){
        return;
    }
    if(!MouseServer::Ref()->is_mouse_in_viewport()){
        return;
    }

    const double zoom_speed = 0.1f;
    const double current_zoom = this->get_zoom();
    vec2 wheel = EngineInputHub::Ref()->get_mouse_wheel();
    double motion = (wheel.y != 0.0f) ? glm::normalize(wheel).y : 0.0f; 
    
    double new_zoom = std::clamp(current_zoom + (zoom_speed * motion), 0.25, 2.0);

    this->set_zoom(new_zoom);
}
void GraphCamera::m_job_draw_border(){
    std::array<glm::vec2, 4> points = get_zoomed_rect().get_points();
    EngineRenderer::Ref()->draw_line(points[0], points[1], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    EngineRenderer::Ref()->draw_line(points[1], points[2], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    EngineRenderer::Ref()->draw_line(points[2], points[3], vec4(0.0f,0.0f,0.0f,1.0f), 3);
    EngineRenderer::Ref()->draw_line(points[3], points[0], vec4(0.0f,0.0f,0.0f,1.0f), 3);
}
void GraphCamera::update(){
    m_job_update_is_dragging();
    m_job_update_control_motion();
    m_job_update_control_zoom();
    m_job_update_window_size_buffer();
    m_job_update_view();
    m_job_update_projection();
    m_job_draw_border();
}
void GraphCamera::set_target(const vec2& p_target){
    m_rect.set_center(p_target);
    refresh_left_top_buffer();
}
void GraphCamera::set_zoom(const float& p_zoom){
    m_zoom = p_zoom;
    refresh_left_top_buffer();
}

vec2 GraphCamera::get_origin() const{
    return get_rect().get_left_top();
}
vec2 GraphCamera::get_target() const{
    return get_rect().get_center();
}
vec2 GraphCamera::get_size() const{
    return get_rect().get_size();
}
vec2 GraphCamera::get_zoomed_size() const{
    return vec4(get_rect().get_size() / m_zoom, 0.0f, 1.0f);
}
float GraphCamera::get_zoom() const{
    return m_zoom;
}
Rect2 GraphCamera::get_zoomed_rect() const{
    Rect2 ret = get_rect();
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
    Rect2 zoomed_rect = get_zoomed_rect();
    vec2 zoomed_size = zoomed_rect.get_size();

    vec2 world_anchor = zoomed_rect.get_left_top();

    vec2 wd_changed_anchor = p_pos - world_anchor;
    vec2 changed_ratio = {
        wd_changed_anchor.x / zoomed_size.x,
        wd_changed_anchor.y / zoomed_size.y
    };

    Rect2 viewport_rect2 = GraphViewport::Ref()->get_viewport_rect();
    vec2 viewport_size = viewport_rect2.get_size();

    vec2 viewport_anchor = viewport_rect2.get_left_top();
    vec2 vp_changed_anchor = {
        changed_ratio.x * viewport_size.x,
        changed_ratio.y * viewport_size.y
    };

    vec2 ret = viewport_anchor + vp_changed_anchor;

    ret = ret - viewport_anchor;

    return ret;
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
    transform = glm::translate(transform, vec3(zoomed_rect.get_left_top(),0.0f));
    vec2 world_pos = transform * vec4(converted_pos, 0.0f, 1.0f);

    return world_pos;
}


vec2 GraphCamera::get_left_top_buffer() const{
    return m_left_top_buffer;
}
void GraphCamera::refresh_left_top_buffer(){
    m_left_top_buffer = this->get_zoomed_rect().get_left_top();
}
void GraphCamera::go_to_left_top_buffer(){
    vec2 new_target = m_left_top_buffer + (this->get_zoomed_rect().get_size() / 2.0f);
    this->set_target(new_target);
}

bool GraphCamera::is_rect_on_camera(Rect2 p_rect, bool p_is_full_needed){
    if(p_is_full_needed){
        Rect2 camera_rect = this->get_zoomed_rect();
        return camera_rect.is_rect_in(p_rect);
    }else{
        Rect2 camera_rect = this->get_zoomed_rect();
        return camera_rect.is_rect_intersect(p_rect);
    }
}

bool GraphCamera::is_rect_id_on_camera(ShapeId p_sid, bool p_is_full_needed){
    bool ret = PhysicsServer::Ref()->is_shape_in_same_chunk(p_sid, m_shape_id);
    return ret;
}

void GraphCamera::upload_data_to_project_server(){
    {
        ProjectPayload payload;
        payload.workspace = ProjectServer::Ref()->current_workspace_uid();
        payload.keys.push_back("camera");
        payload.keys.push_back("position");
        payload.keys.push_back("x");
        ProjectServer::Ref()->set(payload, this->get_target().x);
    }
    {
        ProjectPayload payload;
        payload.workspace = ProjectServer::Ref()->current_workspace_uid();
        payload.keys.push_back("camera");
        payload.keys.push_back("position");
        payload.keys.push_back("y");
        ProjectServer::Ref()->set(payload, this->get_target().y);
    }
    {
        ProjectPayload payload;
        payload.workspace = ProjectServer::Ref()->current_workspace_uid();
        payload.keys.push_back("camera");
        payload.keys.push_back("zoom");
        ProjectServer::Ref()->set(payload, this->get_zoom());
    }
}

