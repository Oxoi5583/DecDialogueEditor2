#include "obj/graph/base.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "graph/camera.h"
#include "imgui/imgui.h"
#include "obj/graph/manager.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"
#include <string>


Rect2& GraphBase::m_init_shape(){
    Rect2 shape = {vec2(0.0f, 0.0f), vec2(50.0f, 50.0f)};
    this->set_shape(shape);
    return this->get_shape<Rect2>();
}

GraphBase::GraphBase()
: m_rect(m_init_shape()){
    BIND_CLASS(GraphBase);
}
GraphBase::~GraphBase(){
    GraphManager::Ref()->notify_name_removed(m_name);
}

vec2 GraphBase::get_position() const{
    return m_rect.get_position();
}
vec2 GraphBase::get_size() const{
    return m_rect.get_size();
}
void GraphBase::set_position(vec2& p_position){
    m_rect.set_position(p_position);
}
void GraphBase::set_size(vec2& p_size){
    m_rect.set_size(p_size);
}

bool GraphBase::is_point_intersect(vec2& p_point){
    return m_rect.is_point_intersect(p_point);
}

void GraphBase::ready(){
    GraphManager::NodeType type = this->get_type();
    std::string default_name = GraphManager::Ref()->get_default_name(type);
    set_name(default_name);
}
void GraphBase::pre_process(){}
void GraphBase::process(){}
void GraphBase::post_process(){}
void GraphBase::draw(){
    if(!is_on_camera()){
        return;
    }

    const float borderSize = 2.0f / GraphCamera::Ref()->get_zoom();
    Rect2 borderRect(
        m_rect.get_center(),
        m_rect.get_size() + vec2(borderSize * 2.0f)
    );

    ImVec4 rect_colour;
    switch(get_type()){
        case GraphManager::BASE:
            rect_colour = ThemeLoader::Ref()->get_imgui_color("SecondaryColour1");
            break;
        case GraphManager::ENTRY:
            rect_colour = ThemeLoader::Ref()->get_imgui_color("EntryColour");
            break;
        case GraphManager::NODE:
            rect_colour = ThemeLoader::Ref()->get_imgui_color("NodeColour");
            break;
        case GraphManager::OPTION:
            rect_colour = ThemeLoader::Ref()->get_imgui_color("OptionColour");
            break;
        default:
            rect_colour = ThemeLoader::Ref()->get_imgui_color("SecondaryColour1");
            break;
    }

    EngineRenderer::Ref()->draw_rect(
        borderRect,
        vec4( rect_colour.x * 0.2f, rect_colour.y * 0.2f, rect_colour.z * 0.2f, rect_colour.w * 1.0f),
        -1
    );

    vec4 fillColor = this->was_clicked()
        ? vec4( rect_colour.x * 1.0f, rect_colour.y * 1.0f, rect_colour.z * 1.0f, rect_colour.w * 1.0f)
        : vec4( rect_colour.x * 0.7f, rect_colour.y * 0.7f, rect_colour.z * 0.7f, rect_colour.w * 0.7f);

    EngineRenderer::Ref()->draw_rect(
        m_rect,
        fillColor,
        -1
    );

    if(this->is_selected()){
        const double width = 6.5 / GraphCamera::Ref()->get_zoom();

        vec2 lt = m_rect.get_left_top();
        vec2 rt = m_rect.get_right_top();
        vec2 rd = m_rect.get_right_down();
        vec2 ld = m_rect.get_left_down();

        vec4 border_color = ThemeLoader::Ref()->get_color("SelectableHighlightColour");

        EngineRenderer::Ref()->draw_line(lt, rt, border_color, width);
        EngineRenderer::Ref()->draw_line(rt, rd, border_color, width);
        EngineRenderer::Ref()->draw_line(rd, ld, border_color, width);
        EngineRenderer::Ref()->draw_line(ld, lt, border_color, width);
    }
}

GraphManager::NodeType GraphBase::get_type(){
    return GraphManager::NodeType::BASE;
}

std::string GraphBase::get_name(){
    return m_name;
}
std::string GraphBase::get_content(){
    return m_content;
}
std::vector<std::string> GraphBase::get_signals(){
    return m_signals;
}
std::vector<OID> GraphBase::get_children(){
    std::vector<OID> ret;
    for(OID id : m_children){
        ret.push_back(id);
    }
    return ret;
}

void GraphBase::set_name(std::string p_name){
    p_name = GraphManager::Ref()->new_name_if_duplicated(p_name);
    m_name = p_name;
    GraphManager::Ref()->notify_name_added(m_name);
}
void GraphBase::set_content(std::string p_content){
    m_content = p_content;
}
void GraphBase::add_signals(){
    m_signals.push_back("");
}
void GraphBase::remove_signals(int p_index){
    if(p_index < 0 || p_index >= m_signals.size()){
        return;
    }
    
    m_signals.erase(m_signals.begin() + p_index);
}
void GraphBase::set_signal(int p_index, std::string p_signal){
    if(p_index < 0 || p_index >= m_signals.size()){
        return;
    }
    
    m_signals[p_index] = p_signal;
}
void GraphBase::add_children(OID p_id){
    if(m_children.contains(p_id)){
        return;
    }
    m_children.emplace(p_id);
}
void GraphBase::remove_children(OID p_id){
    if(!m_children.contains(p_id)){
        return;
    }
    m_children.erase(p_id);
}

