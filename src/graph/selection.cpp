#include "graph/selection.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"


void GraphSelection::m_update_state(){
    switch (m_state) {
        case IDLE:{
            if(EventServer::Ref()->has<EventMouseJustClickedOnWorld>()){
                auto event = EventServer::Ref()->poll_first<EventMouseJustClickedOnWorld>();
                if(event.button == (int)MouseButton::LEFT){
                    m_update_state_TO_DRAGGING();
                }
            }
            break;
        }
        case DRAGGING:{
            if(MouseServer::Ref()->is_just_released()){
                m_update_state_TO_IDLE();
            }
            break;
        }
    }
}

void GraphSelection::m_process(){
    switch (m_state) {
        case IDLE:{
            break;
        }
        case DRAGGING:{
            m_end_dragging_pos = MouseServer::Ref()->get_mouse_world_position();
            vec2 start = glm::max(m_start_dragging_pos,m_end_dragging_pos);
            vec2 end = glm::min(m_start_dragging_pos,m_end_dragging_pos);
            m_selection_area.set_AABB(start, end);
            break;
        }
    }
}

void GraphSelection::m_update_state_TO_IDLE(){
    m_state = State::IDLE;
}
void GraphSelection::m_update_state_TO_DRAGGING(){
    m_state = State::DRAGGING;
    m_start_dragging_pos = MouseServer::Ref()->get_mouse_world_position();
}

void GraphSelection::init(){

}
void GraphSelection::pre_update(){
    if(m_state == State::DRAGGING){
        EventServer::Ref()->emit(EventMouseHoverObj());
    }
}

void GraphSelection::post_update(){
    m_update_state();
    m_process();
}

void GraphSelection::draw(){
    if(m_state != State::DRAGGING){
        return;
    }

    auto points = m_selection_area.get_points();

    vec4 border_color = ThemeLoader::Ref()->get_color("SelectAreaBorderColour");
    vec4 area_color = ThemeLoader::Ref()->get_color("SelectAreaColour");

    EngineRenderer::Ref()->draw_rect(m_selection_area, area_color, -1);

    const double width = 1;

    EngineRenderer::Ref()->draw_line(points[0], points[1], border_color, width);
    EngineRenderer::Ref()->draw_line(points[1], points[2], border_color, width);
    EngineRenderer::Ref()->draw_line(points[2], points[3], border_color, width);
    EngineRenderer::Ref()->draw_line(points[3], points[0], border_color, width);

}

bool GraphSelection::is_selecting(){
    return m_state == State::DRAGGING;
}

bool GraphSelection::is_in_area(Rect2& p_rect){
    if(!is_selecting()){
        return false;
    }

    return m_selection_area.is_rect_intersect(p_rect);
}