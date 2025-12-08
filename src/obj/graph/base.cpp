#include "obj/graph/base.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "editor/components/detail_window.h"
#include "graph/camera.h"
#include "graph/connection.h"
#include "imgui/imgui.h"
#include "obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"
#include <string>
#include <vector>


Rect2& GraphBase::m_init_shape(){
    Rect2 shape = {vec2(0.0f, 0.0f), vec2(50.0f, 50.0f)};
    this->set_shape(shape);
    return this->get_shape<Rect2>();
}

GraphBase::GraphBase(){
    m_init_shape();
    BIND_CLASS(GraphBase);
}
GraphBase::~GraphBase(){
    GraphManager::Ref()->notify_name_removed(m_properties["Unique Id"].value);
}

vec2 GraphBase::get_position() const{
    return this->get_shape<Rect2>().get_position();
}
vec2 GraphBase::get_size() const{
    return this->get_shape<Rect2>().get_size();
}
void GraphBase::set_position(vec2& p_position){
    this->get_shape<Rect2>().set_position(p_position);
}
void GraphBase::set_size(vec2& p_size){
    this->get_shape<Rect2>().set_size(p_size);
}

bool GraphBase::is_point_intersect(vec2& p_point){
    return this->get_shape<Rect2>().is_point_intersect(p_point);
}

void GraphBase::ready(){
    GraphManager::NodeType type = this->get_type();
    std::string default_name = GraphManager::Ref()->get_default_name(type);
    set_name(default_name);
}
void GraphBase::pre_process(){
    m_handle_event_connect();
}
void GraphBase::process(){}
void GraphBase::post_process(){}
void GraphBase::draw(){
    if(!is_on_camera()){
        return;
    }

    const float borderSize = 2.0f / GraphCamera::Ref()->get_zoom();
    Rect2 borderRect(
        this->get_shape<Rect2>().get_center(),
        this->get_shape<Rect2>().get_size() + vec2(borderSize * 2.0f)
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
        case GraphManager::REPEATER:
            rect_colour = ThemeLoader::Ref()->get_imgui_color("AccentColour1");
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
        this->get_shape<Rect2>(),
        fillColor,
        -1
    );

    if(this->is_selected()){
        const double width = 6.5 / GraphCamera::Ref()->get_zoom();

        vec2 lt = this->get_shape<Rect2>().get_left_top();
        vec2 rt = this->get_shape<Rect2>().get_right_top();
        vec2 rd = this->get_shape<Rect2>().get_right_down();
        vec2 ld = this->get_shape<Rect2>().get_left_down();

        vec4 border_color = ThemeLoader::Ref()->get_color("SelectableHighlightColour");

        EngineRenderer::Ref()->draw_line(lt, rt, border_color, width);
        EngineRenderer::Ref()->draw_line(rt, rd, border_color, width);
        EngineRenderer::Ref()->draw_line(rd, ld, border_color, width);
        EngineRenderer::Ref()->draw_line(ld, lt, border_color, width);
    }

    auto connectables = EventServer::Ref()->poll_first<EventTryConnectTo>().conntectables;
    if(connectables.contains(this->get_id())){
        const double width = 6.5 / GraphCamera::Ref()->get_zoom();

        vec2 lt = this->get_shape<Rect2>().get_left_top();
        vec2 rt = this->get_shape<Rect2>().get_right_top();
        vec2 rd = this->get_shape<Rect2>().get_right_down();
        vec2 ld = this->get_shape<Rect2>().get_left_down();

        vec4 border_color = ThemeLoader::Ref()->get_color("AccentColour2");

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
    return m_properties["Unique Id"].value;
}
std::vector<std::string> GraphBase::get_signals(){
    return m_signals;
}
std::vector<OID> GraphBase::get_children(bool is_pass_repeater){
    std::vector<OID> ancestor_ids = this->skip_from_repeater();

    std::vector<OID> ret;

    for(OID& ancestor_id : ancestor_ids){
        GraphBase* ancestor = ObjectServer::Ref()->get_instance<GraphBase>(ancestor_id);
        if(!ancestor){
            return {get_id()};
        }


        std::set<OID> children = ancestor->m_children;
        for(OID id : children){
            GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);

            if(obj->get_type() == GraphManager::REPEATER && is_pass_repeater){
                std::vector<OID> sub_children = obj->skip_to_repeater();
                for(OID sub_id : sub_children){
                    ret.push_back(sub_id);
                }
            }else{
                ret.push_back(id);
            }
        }
    }
    return ret;
}
std::vector<OID> GraphBase::get_parent(bool is_pass_repeater){
    std::vector<OID> ret;
    if(!is_pass_repeater){
        for(OID id : m_parent){
            ret.push_back(id);
        }
        return ret;
    }

    std::vector<OID> ancestor_ids = this->skip_from_repeater();

    for(OID& ancestor_id : ancestor_ids){
        GraphBase* ancestor = ObjectServer::Ref()->get_instance<GraphBase>(ancestor_id);
        if(!ancestor){
            return {get_id()};
        }

        for(OID id : ancestor->m_parent){
            GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);

            if(obj->get_type() == GraphManager::REPEATER){
                std::vector<OID> sub_parent = obj->skip_from_repeater();
                for(OID& sp_id : sub_parent){
                    ret.push_back(sp_id);
                }
            }else{
                ret.push_back(id);
            }
        }
    }

    return ret;
}
std::set<OID> GraphBase::get_children_set(bool is_pass_repeater){
    std::vector<OID> ancestor_ids = this->skip_from_repeater();

    std::set<OID> ret;

    for(OID& ancestor_id : ancestor_ids){
        GraphBase* ancestor = ObjectServer::Ref()->get_instance<GraphBase>(ancestor_id);
        if(!ancestor){
            return {get_id()};
        }


        std::set<OID> children = ancestor->m_children;
        for(OID id : children){
            GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);

            if(obj->get_type() == GraphManager::REPEATER && is_pass_repeater){
                std::vector<OID> sub_children = obj->skip_to_repeater();
                for(OID sub_id : sub_children){
                    ret.emplace(sub_id);
                }
            }else{
                ret.emplace(id);
            }
        }
    }
    return ret;
}
std::set<OID> GraphBase::get_parent_set(bool is_pass_repeater){
    std::set<OID> ret;
    if(!is_pass_repeater){
        for(OID id : m_parent){
            ret.emplace(id);
        }
        return ret;
    }

    std::vector<OID> ancestor_ids = this->skip_from_repeater();

    for(OID& ancestor_id : ancestor_ids){
        GraphBase* ancestor = ObjectServer::Ref()->get_instance<GraphBase>(ancestor_id);
        if(!ancestor){
            return {get_id()};
        }

        for(OID id : ancestor->m_parent){
            GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);

            if(obj->get_type() == GraphManager::REPEATER){
                std::vector<OID> sub_parent = obj->skip_from_repeater();
                for(OID& sp_id : sub_parent){
                    ret.emplace(sp_id);
                }
            }else{
                ret.emplace(id);
            }
        }
    }

    return ret;
}

void GraphBase::set_name(std::string p_name){
    p_name = GraphManager::Ref()->new_name_if_duplicated(p_name);
    m_properties["Unique Id"].value = p_name;
    GraphManager::Ref()->notify_name_added(m_properties["Unique Id"].value);
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
void GraphBase::add_parent(OID p_id){
    if(m_parent.contains(p_id)){
        return;
    }
    m_parent.emplace(p_id);
}
void GraphBase::remove_parent(OID p_id){
    if(!m_parent.contains(p_id)){
        return;
    }
    m_parent.erase(p_id);
}


void GraphBase::m_handle_event_connect(){
    if(EventServer::Ref()->has<EventOpenDetailsWindow>()){
        auto events = EventServer::Ref()->poll<EventOpenDetailsWindow>();
        for(auto event : events){
            if(get_id() == event.id){
                this->open_details_window();
            }
        }
    }

    if(EventServer::Ref()->has<EventStartConnect>()){
        EventStartConnect event = EventServer::Ref()->poll_first<EventStartConnect>();
    }

    if(EventServer::Ref()->has<EventCreateConnection>()){
        auto events = EventServer::Ref()->poll<EventCreateConnection>();
        for(auto event : events){
            if(event.fm_id == this->get_id()){
                add_children(event.to_id);
            }
            if(event.to_id == this->get_id()){
                add_parent(event.fm_id);
            }
        }
    }
    
    if(EventServer::Ref()->has<EventRemoveConnection>()){
        auto events = EventServer::Ref()->poll<EventRemoveConnection>();
        for(auto event : events){
            if(event.fm_id == this->get_id()){
                remove_children(event.to_id);
            }
            if(event.to_id == this->get_id()){
                remove_parent(event.fm_id);
            }
        }
    }
}

std::vector<OID> GraphBase::skip_from_repeater(){
    if(this->m_parent.empty() || this->get_type() != GraphManager::REPEATER){
        return {this->get_id()};
    }
    
    std::vector<OID> ret;
    
    std::set<OID> parent = this->m_parent;
    for(OID id : parent){
        GraphBase* g_obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(g_obj){
            std::vector<OID> cc = g_obj->skip_from_repeater();
            for(OID& c_id : cc){
                ret.push_back(c_id);
            }
        }
    }

    if(ret.empty()){
        return {this->get_id()};
    }else{
        return ret;
    }
}
std::vector<OID> GraphBase::skip_to_repeater(){
    if(this->m_children.empty() || this->get_type() != GraphManager::REPEATER){
        return {this->get_id()};
    }
    
    std::vector<OID> ret;
    
    std::set<OID> children = this->m_children;
    for(OID id : children){
        GraphBase* g_obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(g_obj){
            std::vector<OID> cc = g_obj->skip_to_repeater();
            for(OID& c_id : cc){
                ret.push_back(c_id);
            }
        }
    }

    if(ret.empty()){
        return {this->get_id()};
    }else{
        return ret;
    }
}
std::vector<GraphBase::Property> GraphBase::get_properties(){
    std::vector<GraphBase::Property> ret;
    for(auto it : m_properties){
        ret.push_back(it.second);
    }
    return ret;
}
void GraphBase::open_details_window(){
    EditorDetailsWindow* m_details_window = ObjectServer::Ref()->queue_create<EditorDetailsWindow>(ObjectServer::Layer::UI_LAYER);
    vec2 size = m_details_window->get_shape<Rect2>().get_size();
    m_details_window->set_position(this->get_position() + vec2(50.0f, 100.0f) + size / 2.0f);
    m_details_window->open_for(get_id());
}

void GraphBase::add_property(std::string name, std::string value, uint max_size){
    if(m_properties.contains(name)){
        return;
    }

    m_properties.push_back(
        name,
        {
            name,
            value,
            max_size,
        }
    );
}

void GraphBase::set_property(std::string name, std::string value, uint max_size){
    if(!m_properties.contains(name)){
        return;
    }

    if(name == "Unique Id"){
        set_name(value);
        return;
    }

    m_properties[name] = {
        name,
        value,
        max_size
    };
}
