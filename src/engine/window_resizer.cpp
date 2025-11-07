#include "engine/window_resizer.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/input_hub.h"
#include "engine/window.h"
#include "ext/debug/messenger_ext.h"
#include "engine/renderer.h"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "struct/shape/rect2.h"


void EngineWindowResizer::init(){
    m_refresh_resizer();
}
void EngineWindowResizer::update(){
    m_refresh_resizer();
    m_refresh_collision();
    m_refresh_is_dragging();
    m_refresh_emit_dragging_event();
}

void EngineWindowResizer::m_refresh_resizer(){
    /*
    if(!EventServer::Ref()->has<EventWindowResized>()){
        return;
    }

    EventWindowResized event = EventServer::Ref()->poll_first<EventWindowResized>();
    vec2 new_size = event.new_size;*/

    vec2 new_size = EngineWindow::Ref()->get_window_size();
    Rect2 rect = Rect2(new_size / 2.0f, new_size);

    vec2 left_top = rect.get_left_top();
    vec2 left_down = rect.get_left_down();
    vec2 right_top = rect.get_right_top();
    vec2 right_down = rect.get_right_down();

    const double radius = 10;
    m_left_top_angle.set_radius(radius);
    m_left_down_angle.set_radius(radius);
    m_right_top_angle.set_radius(radius);
    m_right_down_angle.set_radius(radius);
    
    m_left_top_angle.set_position(left_top);
    m_left_down_angle.set_position(left_down);
    m_right_top_angle.set_position(right_top);
    m_right_down_angle.set_position(right_down);
    
    const double width = 10;
    vec2 up_edge_size = vec2(right_top.x,width);
    vec2 up_edge_pos = left_top + (up_edge_size / 2.0f);
    m_up_edge.set_size(up_edge_size);
    m_up_edge.set_position(up_edge_pos);

    vec2 down_edge_size = vec2(right_down.x,width);
    vec2 down_edge_pos = right_down - (down_edge_size / 2.0f);
    m_down_edge.set_size(down_edge_size);
    m_down_edge.set_position(down_edge_pos);

    vec2 left_edge_size = vec2(width, left_down.y);
    vec2 left_edge_pos = left_top + (left_edge_size / 2.0f);
    m_left_edge.set_size(left_edge_size);
    m_left_edge.set_position(left_edge_pos);

    vec2 right_edge_size = vec2(width, right_down.y);
    vec2 right_edge_pos = right_down - (right_edge_size / 2.0f);
    m_right_edge.set_size(right_edge_size);
    m_right_edge.set_position(right_edge_pos);
    /*
    EngineRenderer::Ref()->draw_rect(m_up_edge, vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_rect(m_down_edge, vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_rect(m_left_edge, vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_rect(m_right_edge, vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_circle(m_left_top_angle.get_position(),m_left_top_angle.get_radius(), vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_circle(m_left_down_angle.get_position(),m_left_down_angle.get_radius(), vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_circle(m_right_top_angle.get_position(),m_right_top_angle.get_radius(), vec4(1.0f,0.0f,0.0f,1.0f), -1);
    EngineRenderer::Ref()->draw_circle(m_right_down_angle.get_position(),m_right_down_angle.get_radius(), vec4(1.0f,0.0f,0.0f,1.0f), -1);
    */
}
void EngineWindowResizer::m_refresh_collision(){
    if(!MouseServer::Ref()->is_mouse_in_window()){
        return;
    }

    if(EngineWindow::Ref()->is_maximized()){
        return;
    }

    if(EngineWindow::Ref()->is_minimized()){
        return;
    }

    vec2 mouse_pos = MouseServer::Ref()->get_mouse_screen_position();
    
    for(auto& [angle, dir] : m_angles){
        if(angle->is_point_intersect(mouse_pos)){

            EventMouseHoverObj event;
            event.hovering_pos = mouse_pos;
            EventServer::Ref()->emit(event);

            EventMouseOnResizer event2;
            event2.dir = dir;
            EventServer::Ref()->emit(event2);
            return;
        }
    }

    for(auto& [edge, dir] : m_edges){
        if(edge->is_point_intersect(mouse_pos)){

            EventMouseHoverObj event;
            event.hovering_pos = mouse_pos;
            EventServer::Ref()->emit(event);

            EventMouseOnResizer event2;
            event2.dir = dir;
            EventServer::Ref()->emit(event2);

            return;
        }
    }

}

void EngineWindowResizer::m_refresh_is_dragging(){
    if(MouseServer::Ref()->is_just_released()){ 
        m_is_dragging = false;
        return;
    }
    if(!EventServer::Ref()->has<EventMouseOnResizer>()){
        return;
    }
    if(MouseServer::Ref()->is_just_clicked()){
        if(!m_is_dragging){
            m_is_dragging = true;
            m_dragging_dir = EventServer::Ref()->poll_first<EventMouseOnResizer>().dir;
        }
    }
}

void EngineWindowResizer::m_refresh_emit_dragging_event(){
    if(m_is_dragging){
        float screen_x, screen_y;
        SDL_GetGlobalMouseState(&screen_x, &screen_y);

        EventDragResizer event;
        event.dir = m_dragging_dir;
        event.global_mouse_pos = {screen_x, screen_y};

        EventServer::Ref()->emit(event);
    }
}