#include "graph/viewport.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "engine/window.h"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"
#include <algorithm>

void GraphViewport::init(){
    m_window_size = EngineWindow::Ref()->get_window_size();
    m_viewport_rect = Rect2();
    m_viewport_position_buffer = vec2();
    m_viewport_size_buffer = m_window_size / 2.0f;
}

void GraphViewport::update(){
    m_window_size = EngineWindow::Ref()->get_window_size();
    m_viewport_rect.set_left_top({
        std::clamp(m_viewport_position_buffer.x ,0.0f, m_window_size.x),
        std::clamp(m_viewport_position_buffer.y ,0.0f, m_window_size.y)
    });
    m_viewport_rect.set_size({
        std::clamp(m_viewport_size_buffer.x ,0.0f, m_window_size.x),
        std::clamp(m_viewport_size_buffer.y ,0.0f, m_window_size.y)
    });
}

Rect2 GraphViewport::get_viewport_rect(){
    return m_viewport_rect;
}

void GraphViewport::set_viewport_position(vec2 p_pos){
    m_viewport_position_buffer = p_pos;
}
vec2 GraphViewport::get_viewport_position(){
    return m_viewport_position_buffer;
}

void GraphViewport::set_viewport_size(vec2 p_size){
    m_viewport_size_buffer = p_size;
}
vec2 GraphViewport::get_viewport_size(){
    return m_viewport_size_buffer;
}

vec2 GraphViewport::get_viewport_start(){
    return m_viewport_rect.get_left_top();
}
vec2 GraphViewport::get_viewport_end(){
    return m_viewport_rect.get_right_down();
}

void GraphViewport::set_viewport(double p_start_x, double p_end_x, double p_start_y, double p_end_y){
    double start_x = std::min(p_start_x,p_end_x);
    double start_y = std::min(p_start_y,p_end_y);
    double end_x = std::max(p_start_x, p_end_x);
    double end_y = std::max(p_start_y, p_end_y);
    

    vec2 pos = {start_x, start_y};
    vec2 size = {end_x - start_x, end_y - start_y};
    set_viewport_position(pos);
    set_viewport_size(size);
}

vec2 GraphViewport::screen_to_viewport(vec2 p_pos){
    return p_pos - m_viewport_rect.get_left_top();
}
vec2 GraphViewport::viewport_to_screen(vec2 p_pos){
    return p_pos + m_viewport_rect.get_left_top();
}


