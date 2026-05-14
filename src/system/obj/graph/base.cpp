#include "system/obj/graph/base.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "server/project_server.h"
#include "server/timer_server.h"
#include "editor/components/detail_window.h"
#include "engine/window.h"
#include "system/graph/camera.h"
#include "system/graph/connection.h"
#include "imgui/imgui.h"
#include "system/obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"
#include <cstddef>
#include <string>
#include <vector>


Rect2& GraphBase::m_init_shape(){
    Rect2 shape = {vec2(0.0f, 0.0f), vec2(50.0f, 50.0f)};
    this->set_shape(shape);
    return this->get_shape<Rect2>();
}

void GraphBase::m_update_mouse_on_time(){
    if(this->was_hovered()){
        m_mouse_on_time += EngineWindow::Ref()->get_delta();
    }else{
        m_mouse_on_time = 0.0f;
    }
}

std::string GraphBase::get_uid(){
    return m_uid;
}
void GraphBase::set_uid(std::string p_uid){
    m_uid = p_uid;
}

GraphBase::GraphBase(){
    m_init_shape();
    BIND_CLASS(GraphBase);
    m_workspace_id = ProjectServer::Ref()->current_workspace_uid();
    m_uid = RandomCode(25).get();
}
GraphBase::~GraphBase(){
    GraphManager::Ref()->notify_name_removed(m_properties["Node Name"].value);
    m_remove_project_data();
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

ProjectPayload GraphBase::m_get_root_project_data_payload(){
    ProjectPayload payload;
    payload.workspace = m_workspace_id;
    payload.keys.push_back("objects");
    payload.keys.push_back(m_uid);
    return payload;
}
void GraphBase::m_update_project_data(){
    {
        ProjectPayload payload = m_get_root_project_data_payload();
        payload.keys.push_back("position");
        payload.keys.push_back("x");
        ProjectServer::Ref()->set(payload, this->get_position().x);
    }
    {
        ProjectPayload payload = m_get_root_project_data_payload();
        payload.keys.push_back("position");
        payload.keys.push_back("y");
        ProjectServer::Ref()->set(payload, this->get_position().y);
    }
    {
        ProjectPayload payload = m_get_root_project_data_payload();
        payload.keys.push_back("type");
        ProjectServer::Ref()->set(payload, this->get_type_name());
    }
    {
        ProjectPayload payload = m_get_root_project_data_payload();
        payload.keys.push_back("name");
        ProjectServer::Ref()->set(payload, this->get_name());
    }
    {
        {
            ProjectPayload payload = m_get_root_project_data_payload();
            payload.keys.push_back("children");
            ProjectServer::Ref()->list_clear(payload);
    // Here is a Overflow issue
        }
        for(OID id : this->get_children()){
            ProjectPayload payload = m_get_root_project_data_payload();
            payload.keys.push_back("children");
            GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
            if(obj){
                ProjectServer::Ref()->list_push_back(payload, obj->get_uid());
            }
        }
    }

    GraphManager::Ref()->request_info_refresh(this->get_id());
}

void GraphBase::m_remove_project_data(){
    {
        ProjectPayload payload;
        payload.workspace = m_workspace_id;
        payload.keys.push_back("objects");
        payload.keys.push_back(m_uid);
        ProjectServer::Ref()->remove_key(payload);
    }
}

void GraphBase::m_check_if_upload_project_data_needed(){
    if(this->is_placed()){
        m_update_project_data();
    }
}

void GraphBase::ready(){
    GraphManager::NodeTypeId type = this->get_type();
    std::string default_name = GraphManager::Ref()->get_default_name(type);
    set_name(default_name);
    m_update_project_data();

    switch(get_type()){
        case GraphManager::BASE:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("SecondaryColour1");
            break;
        case GraphManager::ENTRY:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("EntryColour");
            break;
        case GraphManager::NODE:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("NodeColour");
            break;
        case GraphManager::OPTION:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("OptionColour");
            break;
        case GraphManager::REPEATER:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("AccentColour1");
            break;
        case GraphManager::MODULE_ENTRY:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("ModuleEntryColour");
            break;
        case GraphManager::MODULE_NODE:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("ModuleNodeColour");
            break;
        default:
            m_rect_colour = ThemeLoader::Ref()->get_imgui_color("SecondaryColour1");
            break;
    }
}
void GraphBase::pre_process(){
    m_handle_event_connect();
}
void GraphBase::process(){}
void GraphBase::post_process(){
    m_update_mouse_on_time();
    m_check_if_upload_project_data_needed();
}
void GraphBase::draw(){
    if(!is_on_camera()){
        return;
    }

    if(this->is_selected()){
        const double width = 3.5 / GraphCamera::Ref()->get_zoom();

        vec2 lt = this->get_shape<Rect2>().get_left_top();
        vec2 rd = this->get_shape<Rect2>().get_right_down();
        vec2 size = rd - lt;
        vec2 ct = lt + (size / 2.0f);
        
        vec2 lt_hl = lt - vec2(width,width);
        vec2 rd_hl = rd + vec2(width, width);
        vec2 size_hl = rd_hl - lt_hl;

        EngineRenderer::Ref()->draw_rect({ct, size_hl}, m_selected_border_color, -1);
    }

    auto connectables = EventServer::Ref()->poll_first<EventTryConnectTo>().conntectables;
    if(connectables.contains(this->get_id())){
        const double width = 3.5 / GraphCamera::Ref()->get_zoom();
        vec2 lt = this->get_shape<Rect2>().get_left_top();
        vec2 rd = this->get_shape<Rect2>().get_right_down();
        vec2 size = rd - lt;
        vec2 ct = lt + (size / 2.0f);
        
        vec2 lt_hl = lt - vec2(width,width);
        vec2 rd_hl = rd + vec2(width, width);
        vec2 size_hl = rd_hl - lt_hl;        

        EngineRenderer::Ref()->draw_rect({ct, size_hl}, m_connectable_border_color, -1);
    }

    const float borderSize = 2.0f / GraphCamera::Ref()->get_zoom();
    Rect2 borderRect(
        this->get_shape<Rect2>().get_center(),
        this->get_shape<Rect2>().get_size() + vec2(borderSize * 2.0f)
    );

    EngineRenderer::Ref()->draw_rect(
        borderRect,
        vec4( m_rect_colour.x * 0.2f, m_rect_colour.y * 0.2f, m_rect_colour.z * 0.2f, m_rect_colour.w * 1.0f),
        -1
    );

    vec4 fillColor = this->was_clicked()
        ? vec4( m_rect_colour.x * 1.0f, m_rect_colour.y * 1.0f, m_rect_colour.z * 1.0f, m_rect_colour.w * 1.0f)
        : vec4( m_rect_colour.x * 0.7f, m_rect_colour.y * 0.7f, m_rect_colour.z * 0.7f, m_rect_colour.w * 0.7f);

    EngineRenderer::Ref()->draw_rect(
        this->get_shape<Rect2>(),
        fillColor,
        -1
    );
}

GraphManager::NodeTypeId GraphBase::get_type(){
    return GraphManager::NodeTypeId::BASE;
}
std::string GraphBase::get_type_name(){
    return GraphManager::Ref()->type_to_name(this->get_type());
}

std::string GraphBase::get_name(){
    return m_properties["Node Name"].value;
}
std::vector<std::string> GraphBase::get_signals(){
    return m_signals;
}
std::vector<OID> GraphBase::get_children(bool is_pass_repeater, bool is_all){
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
            if(!obj){
                continue;
            }

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

    if(is_all){
        std::vector<OID> all_related;
        for(OID id : ret){
            GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(id);
            if(!ptr){
                continue;
            }

            std::vector<OID> related = ptr->get_children(is_pass_repeater, true);
            for(OID r_id : related){
                all_related.push_back(r_id);
            }
        }
        for(OID r_id : all_related){
            ret.push_back(r_id);
        }
    }

    return ret;
}
std::vector<OID> GraphBase::get_parent(bool is_pass_repeater, bool is_all){
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
            if(!obj){
                continue;
            }

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

    if(is_all){
        std::vector<OID> all_related;
        for(OID id : ret){
            GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(id);
            if(!ptr){
                continue;
            }

            std::vector<OID> related = ptr->get_parent(is_pass_repeater, true);
            for(OID r_id : related){
                all_related.push_back(r_id);
            }
        }
        for(OID r_id : all_related){
            ret.push_back(r_id);
        }
    }

    return ret;
}
std::set<OID> GraphBase::get_children_set(bool is_pass_repeater, bool is_all){
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
            if(!obj){
                continue;
            }

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

    if(is_all){
        std::vector<OID> all_related;
        for(OID id : ret){
            GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(id);
            if(!ptr){
                continue;
            }

            std::vector<OID> related = ptr->get_children(is_pass_repeater, true);
            for(OID r_id : related){
                all_related.push_back(r_id);
            }
        }
        for(OID r_id : all_related){
            ret.emplace(r_id);
        }
    }

    return ret;
}
std::set<OID> GraphBase::get_parent_set(bool is_pass_repeater, bool is_all){
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
            if(!obj){
                continue;
            }

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

    if(is_all){
        std::vector<OID> all_related;
        for(OID id : ret){
            GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(id);
            if(!ptr){
                continue;
            }

            std::vector<OID> related = ptr->get_parent(is_pass_repeater, true);
            for(OID r_id : related){
                all_related.push_back(r_id);
            }
        }
        for(OID r_id : all_related){
            ret.emplace(r_id);
        }
    }

    return ret;
}

std::vector<OID> GraphBase::get_children_direct(){
    std::vector<OID> ret;
    for(auto id : m_children){
        ret.push_back(id);
    }
    return ret;
}

void GraphBase::set_name(std::string p_name){
    OID id = this->get_id();
    p_name = GraphManager::Ref()->new_name_if_duplicated(id,p_name);
    m_properties["Node Name"].value = p_name;
    GraphManager::Ref()->notify_name_added(this->get_id(), m_properties["Node Name"].value);
}
void GraphBase::set_name_forced(std::string p_name){
    m_properties["Node Name"].value = p_name;
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
    m_update_project_data();
}
void GraphBase::remove_children(OID p_id){
    if(!m_children.contains(p_id)){
        return;
    }
    m_children.erase(p_id);
    m_update_project_data();
}
void GraphBase::add_parent(OID p_id){
    if(m_parent.contains(p_id)){
        return;
    }
    m_parent.emplace(p_id);
    m_update_project_data();
}
void GraphBase::remove_parent(OID p_id){
    if(!m_parent.contains(p_id)){
        return;
    }
    m_parent.erase(p_id);
    m_update_project_data();
}

void GraphBase::set_workspace(std::string p_workspace){
    this->m_workspace_id = p_workspace;
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
        if(id != this->get_id() && ObjectServer::Ref()->is_id_valid(id)){
            GraphBase* g_obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
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
        if(id != this->get_id() && ObjectServer::Ref()->is_id_valid(id)){
            GraphBase* g_obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
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

void GraphBase::set_property(std::string name, std::string value, uint max_size){
    if(!m_properties.contains(name)){
        m_properties.push_back(
            name,
            {
                name,
                value,
                max_size,
            }
        );

        return;
    }

    if(name == "Node Name"){
        set_name(value);
        return;
    }

    m_properties[name] = {
        name,
        value,
        max_size
    };

    m_upload_property(name, value, max_size);
}

void GraphBase::m_upload_property(std::string& key, std::string value, uint max_size){
    ProjectPayload obj_root = m_get_root_project_data_payload();
    obj_root.keys.push_back("properties");
    obj_root.keys.push_back(key);

    ProjectPayload val_pl = obj_root;
    val_pl.keys.push_back("value");
    ProjectServer::Ref()->set(val_pl, value);

    ProjectPayload size_pl = obj_root;
    size_pl.keys.push_back("max_size");
    ProjectServer::Ref()->set(size_pl, (int)max_size);
}

std::string GraphBase::get_property(std::string p_name){
    if(!m_properties.contains(p_name)){
        return "";
    }
    return m_properties[p_name].value;
}

bool GraphBase::is_expanded(){
    return m_is_expanded;
}
void GraphBase::expand_on_list(){
    m_is_expanded = true;
    GraphManager::Ref()->request_info_refresh(this->get_id());
}
void GraphBase::collapse_on_list(){
    m_is_expanded = false;
    GraphManager::Ref()->request_info_refresh(this->get_id());
}
std::string GraphBase::get_workspace_id(){
    return m_workspace_id;
}