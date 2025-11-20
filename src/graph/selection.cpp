#include "graph/selection.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "imgui/imgui.h"
#include "obj/abstract/selectable.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"
#include <algorithm>
#include <vector>


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

void GraphSelection::m_block_hover_if_selecting(){
    if(m_state == State::DRAGGING){
        EventServer::Ref()->block<EventMouseHoverObj>();
    }
}

void GraphSelection::pre_update(){
    m_store_selection();
    m_execute_external_events();
    m_execute_internal_events();
    m_block_hover_if_selecting();
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

void GraphSelection::m_store_selection(){
    std::vector<OID>().swap(m_selected);
    auto selecteds = EventServer::Ref()->poll<EventMouseSelectedObj>();
    for(auto& s : selecteds){
        m_selected.push_back(s.obj_id);
    }
}

void GraphSelection::m_drag_all_selection(){
    for(OID& id : m_selected_group_dragging_buffer){
        SelectableObject* obj = ObjectServer::Ref()->get_instance<SelectableObject>(id);
        obj->select();
        obj->drag();
    }
}
void GraphSelection::m_place_all_selection(){
    for(OID& id : m_selected_group_dragging_buffer){
        SelectableObject* obj = ObjectServer::Ref()->get_instance<SelectableObject>(id);
        obj->select();
        obj->place();
    }
}

void GraphSelection::m_execute_internal_events(){
    while(!m_events.empty()){
        Event e = m_events.front();
        switch (e) {
            case DRAG_ALL_SELECTION:{
                m_drag_all_selection();
                break;
            }
            case PLACE_ALL_SELECTION:{
                m_place_all_selection();
                break;
            }
            case STORE_BUFFER:{
                m_store_selection_buffer();
                break;
            }
        }
        m_events.pop();
    }
}

void GraphSelection::m_execute_external_events(){
    if(EventServer::Ref()->has<EventSelectedObjDragging>()){
        drag_all_selection();
    }
    if(EventServer::Ref()->has<EventSelectedObjPlaced>()){
        place_all_selection();
    }
}
void GraphSelection::drag_all_selection(){
    m_drag_all_selection();
}
void GraphSelection::place_all_selection(){
    m_place_all_selection();
}

bool GraphSelection::is_group_dragging(){
    return m_is_group_dragging;
}

void GraphSelection::m_store_selection_buffer(){
    m_selected_group_dragging_buffer.resize(m_selected.size());
    std::copy(m_selected.begin(), m_selected.end(), m_selected_group_dragging_buffer.begin());
    for(OID& id : m_selected_group_dragging_buffer){
        m_selected_group_dragging_buffer_us.emplace(id);
    }
}
void GraphSelection::m_release_selection_buffer(){
    std::vector<OID>().swap(m_selected_group_dragging_buffer);
    m_selected_group_dragging_buffer_us.clear();
}

void GraphSelection::store_selection_buffer(){
    m_events.emplace(Event::STORE_BUFFER);
}
void GraphSelection::release_selection_buffer(){
    m_release_selection_buffer();
}


void GraphSelection::unselect_all(){
    for(OID& id : m_selected){
        if(ObjectServer::Ref()->is_id_valid(id)){
            SelectableObject* obj = ObjectServer::Ref()->get_instance<SelectableObject>(id);
            obj->unselect();
        }
    }
}

std::vector<OID> GraphSelection::get_selected(){
    if(!m_is_group_dragging){
        return m_selected;
    }else{
        return m_selected_group_dragging_buffer;
    }
}

bool GraphSelection::is_id_in_dragging_buffer(OID& p_id){
    return m_selected_group_dragging_buffer_us.contains(p_id);
}