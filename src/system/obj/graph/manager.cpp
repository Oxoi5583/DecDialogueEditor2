#include "system/obj/graph/manager.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "ext/debug/messenger_ext.h"
#include "glm/ext/vector_float2.hpp"
#include "system/obj/graph/module_entry.h"
#include "system/obj/graph/module_node.h"
#include "system/obj/graph/repeater.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <algorithm>
#include <string>
#include <system/graph/connection.h>
#include <unordered_set>
#include <vector>


#include "system/obj/graph/base.h"
#include "system/obj/graph/entry.h"
#include "system/obj/graph/node.h"
#include "system/obj/graph/option.h"

GraphManager::GraphManager(){}
GraphManager::~GraphManager(){}

void GraphManager::m_spawn_entry(EventSpawnNode p_event){
    GraphEntry* new_node = ObjectServer::Ref()->queue_create<GraphEntry>();

    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_event.spawn_pos);

    if(p_event.is_workspace_custom){
        new_node->set_workspace(p_event.custom_workspace);
    }
    if(p_event.is_uid_custom){
        new_node->set_uid(p_event.custom_uid);
    }
    if(p_event.is_name_custom){
        new_node->set_name(p_event.custom_name);
    }
    for(auto& item : p_event.init_data){
        new_node->set_property(item.key, item.value, item.max_size);
    }

    m_uid_to_id.emplace(new_node->get_uid(), new_node->get_id());
    m_id_to_uid.emplace(new_node->get_id(), new_node->get_uid());
}
void GraphManager::m_spawn_node(EventSpawnNode p_event){
    GraphNode* new_node = ObjectServer::Ref()->queue_create<GraphNode>();

    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_event.spawn_pos);

    if(p_event.is_workspace_custom){
        new_node->set_workspace(p_event.custom_workspace);
    }
    if(p_event.is_uid_custom){
        new_node->set_uid(p_event.custom_uid);
    }
    if(p_event.is_name_custom){
        new_node->set_name(p_event.custom_name);
    }
    for(auto& item : p_event.init_data){
        new_node->set_property(item.key, item.value, item.max_size);
    }

    m_uid_to_id.emplace(new_node->get_uid(), new_node->get_id());
    m_id_to_uid.emplace(new_node->get_id(), new_node->get_uid());
}
void GraphManager::m_spawn_option(EventSpawnNode p_event){
    GraphOption* new_node = ObjectServer::Ref()->queue_create<GraphOption>();
    
    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_event.spawn_pos);

    if(p_event.is_workspace_custom){
        new_node->set_workspace(p_event.custom_workspace);
    }
    if(p_event.is_uid_custom){
        new_node->set_uid(p_event.custom_uid);
    }
    if(p_event.is_name_custom){
        new_node->set_name(p_event.custom_name);
    }
    for(auto& item : p_event.init_data){
        new_node->set_property(item.key, item.value, item.max_size);
    }

    m_uid_to_id.emplace(new_node->get_uid(), new_node->get_id());
    m_id_to_uid.emplace(new_node->get_id(), new_node->get_uid());
}
void GraphManager::m_spawn_repeater(EventSpawnNode p_event){
    GraphRepeater* new_node = ObjectServer::Ref()->queue_create<GraphRepeater>();
    
    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_event.spawn_pos);

    if(p_event.is_workspace_custom){
        new_node->set_workspace(p_event.custom_workspace);
    }
    if(p_event.is_uid_custom){
        new_node->set_uid(p_event.custom_uid);
    }
    if(p_event.is_name_custom){
        new_node->set_name(p_event.custom_name);
    }
    for(auto& item : p_event.init_data){
        new_node->set_property(item.key, item.value, item.max_size);
    }

    m_uid_to_id.emplace(new_node->get_uid(), new_node->get_id());
    m_id_to_uid.emplace(new_node->get_id(), new_node->get_uid());
}
void GraphManager::m_spawn_module_entry(EventSpawnNode p_event){
    GraphModuleEntry* new_node = ObjectServer::Ref()->queue_create<GraphModuleEntry>();
    
    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_event.spawn_pos);

    if(p_event.is_workspace_custom){
        new_node->set_workspace(p_event.custom_workspace);
    }
    if(p_event.is_uid_custom){
        new_node->set_uid(p_event.custom_uid);
    }
    if(p_event.is_name_custom){
        new_node->set_name(p_event.custom_name);
    }
    for(auto& item : p_event.init_data){
        new_node->set_property(item.key, item.value, item.max_size);
    }

    m_uid_to_id.emplace(new_node->get_uid(), new_node->get_id());
    m_id_to_uid.emplace(new_node->get_id(), new_node->get_uid());
}
void GraphManager::m_spawn_module_node(EventSpawnNode p_event){
    GraphModuleNode* new_node = ObjectServer::Ref()->queue_create<GraphModuleNode>();
    
    NodeInfo info = m_create_info(new_node);

    m_all_node_ids.push_back(info.id);
    m_non_entry_node_ids.push_back(info.id);
    m_infos.emplace(info.id, info);

    new_node->set_position(p_event.spawn_pos);

    if(p_event.is_workspace_custom){
        new_node->set_workspace(p_event.custom_workspace);
    }
    if(p_event.is_uid_custom){
        new_node->set_uid(p_event.custom_uid);
    }
    if(p_event.is_name_custom){
        new_node->set_name(p_event.custom_name);
    }
    for(auto& item : p_event.init_data){
        new_node->set_property(item.key, item.value, item.max_size);
    }

    m_uid_to_id.emplace(new_node->get_uid(), new_node->get_id());
    m_id_to_uid.emplace(new_node->get_id(), new_node->get_uid());
}

GraphManager::NodeInfo GraphManager::m_create_info(GraphBase* m_ptr){
    NodeInfo info;

    info.id = m_ptr->get_id();
    info.type = m_ptr->get_type();
    info.name = m_ptr->get_name();
    info.is_selected = m_ptr->is_selected();
    info.is_expanded = m_ptr->is_expanded();
    info.signals = m_ptr->get_signals();
    info.direct_children = m_ptr->get_children_direct();

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
    m_type_dict.add<GraphBase>(NodeTypeId::BASE, "Base", "B_NewBase");
    m_type_dict.add<GraphEntry>(NodeTypeId::ENTRY, "Entry", "E_NewEntry");
    m_type_dict.add<GraphOption>(NodeTypeId::OPTION, "Node", "N_NewNode");
    m_type_dict.add<GraphNode>(NodeTypeId::NODE, "Option", "O_NewOption");
    m_type_dict.add<GraphRepeater>(NodeTypeId::REPEATER, "Repeater", "R_NewRepeater");
    m_type_dict.add<GraphModuleEntry>(NodeTypeId::MODULE_ENTRY, "Module Entry", "ME_NewModuleEntry");
    m_type_dict.add<GraphModuleNode>(NodeTypeId::MODULE_NODE, "Module Node", "MN_NewModuleNode");
    m_type_dict.add<GraphBase>(NodeTypeId::UNKNOWN, "Unknown", "U_NewUnknown");
}
void GraphManager::update(){
    m_data_refresh();
    m_poll_spawn_event();
    m_poll_create_uid_connection();
}

void GraphManager::m_poll_spawn_event(){
    auto events = EventServer::Ref()->poll<EventSpawnNode>();
    for(auto event : events){
        switch (event.type) {
            case ENTRY:{
                m_spawn_entry(event);
                break;
            }
            case NODE:{
                m_spawn_node(event);
                break;
            }
            case OPTION:{
                m_spawn_option(event);
                break;
            }
            case REPEATER:{
                m_spawn_repeater(event);
                break;
            }
            case MODULE_ENTRY:{
                m_spawn_module_entry(event);
                break;
            }
            case MODULE_NODE:{
                m_spawn_module_node(event);
                break;
            }
            case BASE:{
                break;
            }
            default:{
                break;
            }
        }
    }
}
void GraphManager::m_poll_create_uid_connection(){
    auto events = EventServer::Ref()->poll<EventCreateConnectionWithUID>();
    for(auto event : events){
        OID fm_id = m_uid_to_id[event.fm_uid];
        OID to_id = m_uid_to_id[event.to_uid];

        GraphConnection::Ref()->create_connection_obj(fm_id, to_id);
    }
}

void GraphManager::m_clear_garbage(){
    std::unordered_set<OID> garbages;
    for(OID& id : m_all_node_ids){
        if(!ObjectServer::Ref()->is_id_valid(id)){
            garbages.emplace(id);
            m_infos.erase(id);
            std::string uid = m_id_to_uid[id];
            m_uid_to_id.erase(uid);
            m_id_to_uid.erase(id);
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
    while(!m_nodes_need_info_refresh.empty()){
        OID id = m_nodes_need_info_refresh.front();
        GraphBase* ptr = ObjectServer::Ref()->get_instance<GraphBase>(id);
        NodeInfo info = m_create_info(ptr);
        m_infos[id] = info;

        m_nodes_need_info_refresh.pop();
    }
}
void GraphManager::request_info_refresh(OID p_id){
    m_nodes_need_info_refresh.emplace(p_id);
}

void GraphManager::m_regenerate_panel_data(){
    std::unordered_set<OID> used_ids;

    m_panel_data = PanelData();
    for(OID& id : m_entry_node_ids){
        if(ObjectServer::Ref()->is_obj_freeze(id)){
            continue;
        }

        used_ids.emplace(id);

        m_panel_data.primary_info_list.push_back(id);
        std::vector<OID> secondary_info_list = std::vector<OID>();
        std::vector<OID> children = m_get_all_children(id, false);
        for(OID& s_id : children){
            if(ObjectServer::Ref()->is_obj_freeze(s_id)){
                continue;
            }

            used_ids.emplace(s_id);
            secondary_info_list.push_back(s_id);
        }

        m_panel_data.secondary_info_list.push_back(secondary_info_list);
    }
    
    for(OID& id : m_all_node_ids){
        if(ObjectServer::Ref()->is_obj_freeze(id)){
            continue;
        }

        if(used_ids.contains(id)){
            continue;
        }

        m_panel_data.other_info_list.push_back(id);
    }
}

void GraphManager::m_data_refresh(){
    m_clear_garbage();
    m_refetch_node_data();
    m_regenerate_panel_data();
}

const GraphManager::PanelData& GraphManager::get_panel_data(){
    return m_panel_data;
}

bool GraphManager::is_name_duplicated(OID p_id, std::string p_name){
    return m_exists_names.contains(p_name)
    && m_exists_names[p_name] != p_id;
}
std::string GraphManager::new_name_if_duplicated(OID p_id, std::string p_name){
    if(!is_name_duplicated(p_id, p_name)){
        return p_name;
    }
    
    int index = 1;
    std::string new_name = p_name + " (" + std::to_string(index) + ")";

    while(is_name_duplicated(p_id,new_name)){
        index++;
        new_name = p_name + " (" + std::to_string(index) + ")";
    }

    return new_name;
}

GraphManager::NodeTypeId GraphManager::TypeDict::get(std::string p_name){
    if(!str_to_type.contains(p_name)){
        return NodeTypeId::UNKNOWN;
    }
    return str_to_type[p_name];
}
std::string GraphManager::TypeDict::get(NodeTypeId p_type){
    if(!type_to_str.contains(p_type)){
        return type_to_str[NodeTypeId::UNKNOWN];
    }
    return type_to_str[p_type];
}
std::string GraphManager::TypeDict::get_default_name(NodeTypeId p_type){
    if(!type_to_default.contains(p_type)){
        return type_to_default[NodeTypeId::UNKNOWN];
    }
    return type_to_default[p_type];
}
OID GraphManager::TypeDict::spawn(NodeTypeId p_type){
    if(!type_to_spawn.contains(p_type)){
        return -1;
    }
    if(p_type == NodeTypeId::UNKNOWN){
        return -1;
    }

    return type_to_spawn[p_type]();
}

std::string GraphManager::get_default_name(NodeTypeId p_type){
    std::string new_name = m_type_dict.get_default_name(p_type);
    return new_name_if_duplicated(-1, new_name);
}

void GraphManager::notify_name_removed(std::string p_name){
    if(m_exists_names.contains(p_name)){
        m_exists_names.erase(p_name);
    }
}

void GraphManager::notify_name_added(OID p_id, std::string p_name){
    if(!m_exists_names.contains(p_name)){
        m_exists_names.emplace(p_name, p_id);
    }
}

std::string GraphManager::type_to_name(NodeTypeId p_type){
    return m_type_dict.get(p_type);
}
GraphManager::NodeTypeId GraphManager::name_to_type(std::string p_name){
    return m_type_dict.get(p_name);
}

OID GraphManager::spawn(NodeTypeId p_type){
    return m_type_dict.spawn(p_type);
}

void GraphManager::clear_nodes(){
    for(OID id : m_all_node_ids){
        ObjectServer::Ref()->get_instance<ObjectBase>(id)->queue_free();
    }
}
const GraphManager::NodeInfo& GraphManager::get_node_info(const OID& p_id){
    return m_infos[p_id];
}