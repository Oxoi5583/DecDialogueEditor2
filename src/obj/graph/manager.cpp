#include "obj/graph/manager.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "ext/debug/messenger_ext.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/graph/repeater.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>


#include "obj/graph/base.h"
#include "obj/graph/entry.h"
#include "obj/graph/node.h"
#include "obj/graph/option.h"

GraphManager::GraphManager(){}
GraphManager::~GraphManager(){}

void GraphManager::m_spawn_entry(vec2 p_pos){
    GraphEntry* new_node = ObjectServer::Ref()->queue_create<GraphEntry>();

    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_pos);
}
void GraphManager::m_spawn_node(vec2 p_pos){
    GraphNode* new_node = ObjectServer::Ref()->queue_create<GraphNode>();

    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_pos);
}
void GraphManager::m_spawn_option(vec2 p_pos){
    GraphOption* new_node = ObjectServer::Ref()->queue_create<GraphOption>();
    
    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_pos);
}
void GraphManager::m_spawn_repeater(vec2 p_pos){
    GraphRepeater* new_node = ObjectServer::Ref()->queue_create<GraphRepeater>();
    
    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_pos);
}

GraphManager::NodeInfo GraphManager::m_create_info(GraphBase* m_ptr){
    NodeInfo info;

    info.id = m_ptr->get_id();
    info.type = m_ptr->get_type();
    info.name = m_ptr->get_name();
    info.is_selected = m_ptr->is_selected();
    info.signals = m_ptr->get_signals();
    info.direct_children = m_ptr->get_children();

    return info;
}

std::vector<OID> GraphManager::m_get_all_children(OID p_parent_id, bool p_is_self_contain, std::unordered_set<OID> p_ancestors){
    std::vector<OID> ret;

    if(p_is_self_contain){
        ret.push_back(p_parent_id);
    }

    p_ancestors.emplace(p_parent_id);

    auto direct_children = m_infos[p_parent_id].direct_children;
    for(OID& id : direct_children){
        if(p_ancestors.contains(id)){
            continue;
        }

        auto sub_children = m_get_all_children(id, true, p_ancestors);
        for(OID& sub_id : sub_children){
            ret.push_back(sub_id);
        }
    }

    return ret;
}

void GraphManager::init(){
    m_data_refresh_timer = TimerServer::Ref()->create_timer(
            TimeUnit(TimeUnit::Type::SECOND, 
            m_data_refresh_second)
        );
}
void GraphManager::update(){
    m_poll_spawn_event();
    m_data_refresh();
}


void GraphManager::m_poll_spawn_event(){
    auto events = EventServer::Ref()->poll<EventSpawnNode>();
    for(auto event : events){
        vec2 spawn_pos = event.spawn_pos;
        switch (event.type) {
            case ENTRY:{
                m_spawn_entry(spawn_pos);
                break;
            }
            case NODE:{
                m_spawn_node(spawn_pos);
                break;
            }
            case OPTION:{
                m_spawn_option(spawn_pos);
                break;
            }
            case REPEATER:
                m_spawn_repeater(spawn_pos);
                break;
            case BASE:
                break;
        }
    }
}

void GraphManager::m_clear_garbage(){
    std::unordered_set<OID> garbages;
    for(OID& id : m_all_node_ids){
        if(!ObjectServer::Ref()->is_id_valid(id)){
            garbages.emplace(id);
            m_infos.erase(id);
        }
    }

    m_all_node_ids.erase(
        std::remove_if(
            m_all_node_ids.begin(),
            m_all_node_ids.end(),
            [garbages](const OID& id){ return garbages.contains(id); }
        ),
        m_all_node_ids.end()
    );

    m_entry_node_ids.erase(
        std::remove_if(
            m_entry_node_ids.begin(),
            m_entry_node_ids.end(),
            [garbages](const OID& id){ return garbages.contains(id); }
        ),
        m_entry_node_ids.end()
    );
    
    m_non_entry_node_ids.erase(
        std::remove_if(
            m_non_entry_node_ids.begin(),
            m_non_entry_node_ids.end(),
            [garbages](const OID& id){ return garbages.contains(id); }
        ),
        m_non_entry_node_ids.end()
    );
}

void GraphManager::m_refetch_node_data(){
    for(OID& id : m_all_node_ids){
        GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(id);
        NodeInfo info = m_create_info(ptr);
        m_infos[id] = info;
    }
}

void GraphManager::m_regenerate_panel_data(){
    std::unordered_set<OID> used_ids;

    m_panel_data = PanelData();
    for(OID& id : m_entry_node_ids){
        used_ids.emplace(id);

        NodeInfo& info = m_infos[id];
        m_panel_data.primary_info_list.push_back(info);
        std::vector<NodeInfo> secondary_info_list = std::vector<NodeInfo>();

        std::vector<OID> children = m_get_all_children(id, false);
        for(OID& s_id : children){
            used_ids.emplace(s_id);

            NodeInfo s_info = m_infos[s_id];
            secondary_info_list.push_back(s_info);
        }

        m_panel_data.secondary_info_list.push_back(secondary_info_list);
    }
    
    for(OID& id : m_all_node_ids){
        if(used_ids.contains(id)){
            continue;
        }

        NodeInfo info = m_infos[id];
        m_panel_data.other_info_list.push_back(info);
    }
}

void GraphManager::m_data_refresh(){
    /*
    if(!m_data_refresh_timer->timeout_and_reset()){
        return;
    }
    */
    
    m_clear_garbage();
    m_refetch_node_data();
    m_regenerate_panel_data();
}

GraphManager::PanelData GraphManager::get_panel_data(){
    return m_panel_data;
}

bool GraphManager::is_name_duplicated(std::string p_name){
    return m_exists_names.contains(p_name);
}
std::string GraphManager::new_name_if_duplicated(std::string p_name){
    if(!is_name_duplicated(p_name)){
        return p_name;
    }

    int index = 1;
    std::string new_name = p_name + " (" + std::to_string(index) + ")";

    while(is_name_duplicated(new_name)){
        index++;
        new_name = p_name + " (" + std::to_string(index) + ")";
    }

    return new_name;
}
std::string GraphManager::get_default_name(NodeType p_type){
    std::string new_name;
    switch (p_type) {
        case BASE:
            new_name = "Base";
            break;
        case ENTRY:
            new_name = "Entry";
            break;
        case NODE:
            new_name = "Node";
            break;
        case OPTION:
            new_name = "Option";
            break;
        case REPEATER:
            new_name = "Repeater";
            break;
        default:
            new_name = "Unknown";
            break;
    }

    return new_name_if_duplicated(new_name);
}

void GraphManager::notify_name_removed(std::string p_name){
    if(m_exists_names.contains(p_name)){
        m_exists_names.erase(p_name);
    }
}
void GraphManager::notify_name_added(std::string p_name){
    if(!m_exists_names.contains(p_name)){
        m_exists_names.emplace(p_name);
    }
}