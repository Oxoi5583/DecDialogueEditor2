#include "graph/viewport.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/input_hub.h"
#include "engine/input_key.h"
#include "ext/debug/messenger_ext.h"
#include "engine/window.h"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"
#include <algorithm>

void GraphViewport::init(){
    m_window_size = EngineWindow::Ref()->get_window_size();
    m_viewport_rect = Rect2();
    m_viewport_rect_buffer = {vec2(), m_window_size / 2.0f};
    m_viewport_rect_buffer = m_viewport_rect;
}

void GraphViewport::update(){
    m_window_size = EngineWindow::Ref()->get_window_size();

    vec2 buffer_left_top = m_viewport_rect_buffer.get_left_top();
    vec2 buffer_right_down = m_viewport_rect_buffer.get_right_down();
        
    vec2 new_right_down = {
        std::clamp(buffer_right_down.x, buffer_left_top.x, m_window_size.x),
        std::clamp(buffer_right_down.y, buffer_left_top.y, m_window_size.y),  
    };
    vec2 new_left_top = {
        std::clamp(buffer_left_top.x, 0.0f, new_right_down.x),
        std::clamp(buffer_left_top.y, 0.0f, new_right_down.y),  
    };

    m_viewport_rect.set_AABB(new_left_top, new_right_down);

    m_viewport_rect_buffer = m_viewport_rect;
}


Rect2 GraphViewport::get_viewport_rect(){
    return m_viewport_rect;
}

void GraphViewport::set_viewport_position(vec2 p_pos){
    m_viewport_rect_buffer.set_left_top(p_pos);
}
vec2 GraphViewport::get_viewport_position(){
    return m_viewport_rect.get_left_top();
}

void GraphViewport::set_viewport_size(vec2 p_size){
    m_viewport_rect_buffer.set_size(p_size);
}
void GraphViewport::set_viewport_center(vec2 p_pos){
    m_viewport_rect_buffer.set_center(p_pos);
}
vec2 GraphViewport::get_viewport_center(){
    return m_viewport_rect.get_center();
}
vec2 GraphViewport::get_viewport_size(){
    return m_viewport_rect.get_size();
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


void GraphViewport::set_viewport_up_border(double p_y){
    vec2 size = m_viewport_rect_buffer.get_size();

    vec2 left_top = m_viewport_rect_buffer.get_left_top();
    vec2 right_down = m_viewport_rect_buffer.get_right_down();

    float y = std::min((float)p_y, right_down.y);
    vec2 new_left_top = {left_top.x, y};
    vec2 new_right_down = right_down;

    m_viewport_rect_buffer.set_AABB(new_left_top, new_right_down);
}
void GraphViewport::set_viewport_left_border(double p_x){
    vec2 size = m_viewport_rect_buffer.get_size();

    vec2 left_top = m_viewport_rect_buffer.get_left_top();
    vec2 right_down = m_viewport_rect_buffer.get_right_down();

    float x = std::min((float)p_x, right_down.x);
    vec2 new_left_top = {x, left_top.y};
    vec2 new_right_down = right_down;

    m_viewport_rect_buffer.set_AABB(new_left_top, new_right_down);
}
void GraphViewport::set_viewport_down_border(double p_y){
    vec2 size = m_viewport_rect_buffer.get_size();

    vec2 left_top = m_viewport_rect_buffer.get_left_top();
    vec2 right_down = m_viewport_rect_buffer.get_right_down();

    float y = std::max((float)p_y, left_top.y);
    vec2 new_left_top = left_top;
    vec2 new_right_down = {right_down.x, y};

    m_viewport_rect_buffer.set_AABB(new_left_top, new_right_down);
}
void GraphViewport::set_viewport_right_border(double p_x){
    vec2 size = m_viewport_rect_buffer.get_size();

    vec2 left_top = m_viewport_rect_buffer.get_left_top();
    vec2 right_down = m_viewport_rect_buffer.get_right_down();

    float x = std::max((float)p_x, left_top.x);
    vec2 new_left_top = left_top;
    vec2 new_right_down = {x, right_down.y};

    m_viewport_rect_buffer.set_AABB(new_left_top, new_right_down);
}

double GraphViewport::get_viewport_up_border(){
    return m_viewport_rect.get_left_top().y;
}
double GraphViewport::get_viewport_down_border(){
    return m_viewport_rect.get_right_down().y;
}
double GraphViewport::get_viewport_left_border(){
    return m_viewport_rect.get_left_top().x;
}
double GraphViewport::get_viewport_right_border(){
    return m_viewport_rect.get_right_down().x;
}
