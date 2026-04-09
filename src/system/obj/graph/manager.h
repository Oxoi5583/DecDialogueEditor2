#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "server/timer_server.h"
#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <functional>
#include <map>
#include <server/object_server.h>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace glm;

class GraphBase;
class GraphEntry;
class GraphOption;
class GraphNode;
class GraphRepeater;
class GraphModuleEntry;
class GraphModuleNode;

struct EventSpawnNode;

class GraphManager : public Singleton<GraphManager> {
public:
    enum NodeTypeId{
        BASE,
        ENTRY,
        NODE,
        OPTION,
        REPEATER,
        MODULE_ENTRY,
        MODULE_NODE,
        UNKNOWN,
    };
    
    struct NodeInfo{
        OID id;
        NodeTypeId type;
        std::string name;
        bool is_selected;
        bool is_expanded;
        std::vector<std::string> signals;
        std::vector<OID> direct_children;
    };

    struct PanelData{
        std::vector<NodeInfo> primary_info_list;
        std::vector<std::vector<NodeInfo>> secondary_info_list;
        std::vector<NodeInfo> other_info_list;
    };
private:
    const double m_data_refresh_second = 1.2f;
    TimerWrapper m_data_refresh_timer = {
            TimeUnit(TimeUnit::Type::SECOND, 
            m_data_refresh_second), true};

    std::vector<OID> m_all_node_ids;
    std::vector<OID> m_entry_node_ids;
    std::vector<OID> m_non_entry_node_ids;
    std::unordered_map<OID, NodeInfo> m_infos;
    std::unordered_map<std::string, OID> m_uid_to_id;
    std::unordered_map<OID, std::string> m_id_to_uid;

    std::unordered_map<std::string,OID> m_exists_names;

    PanelData m_panel_data;

    void m_poll_spawn_event();
    void m_poll_create_uid_connection();
    void m_clear_garbage();
    void m_refetch_node_data();
    void m_regenerate_panel_data();
    void m_data_refresh();

    NodeInfo m_create_info(GraphBase* m_ptr);
    std::vector<OID> m_get_all_children(OID p_parent_id, 
                                        bool p_is_self_contain = true,
                                        std::unordered_set<OID> p_ancestors = {});

    void m_spawn_entry(EventSpawnNode p_event);
    void m_spawn_node(EventSpawnNode p_event);
    void m_spawn_option(EventSpawnNode p_event);
    void m_spawn_repeater(EventSpawnNode p_event);
    void m_spawn_module_entry(EventSpawnNode p_event);
    void m_spawn_module_node(EventSpawnNode p_event);

    struct TypeDict{
        std::map<NodeTypeId , std::string> type_to_str;
        std::map<std::string, NodeTypeId> str_to_type;
        std::map<NodeTypeId , std::string> type_to_default;
        std::map<NodeTypeId , std::function<OID()>> type_to_spawn;

        NodeTypeId get(std::string p_name);
        std::string get(NodeTypeId p_type);
        std::string get_default_name(NodeTypeId p_type);

        template<typename T>
        void add(NodeTypeId p_type, std::string p_name, std::string p_default_name){
            type_to_str.emplace(p_type, p_name);
            str_to_type.emplace(p_name, p_type);
            type_to_default.emplace(p_type, p_default_name);
            std::function<OID()> fn = []() -> OID {
                return ObjectServer::Ref()->queue_create<T>()->get_id();
            };
            type_to_spawn.try_emplace(p_type, fn);
        }

        OID spawn(NodeTypeId p_type);
    } m_type_dict;
public:
    GraphManager();
    ~GraphManager();

    void init();
    void update();

    bool is_name_duplicated(OID p_id, std::string p_name);
    std::string new_name_if_duplicated(OID p_id, std::string p_name);
    std::string get_default_name(NodeTypeId p_type);

    void notify_name_removed(std::string p_name);
    void notify_name_added(OID p_id, std::string p_name);

    const GraphManager::PanelData& get_panel_data();

    std::string type_to_name(NodeTypeId p_type);
    NodeTypeId name_to_type(std::string p_name);

    OID spawn(NodeTypeId p_type);

    void clear_nodes();

    template<typename T>
    void register_type(NodeTypeId p_id, std::string p_name, std::string p_default_name){
        if(m_type_dict.type_to_str.contains(p_id)){
            return;
        }
        m_type_dict.add<T>(p_name, p_default_name);
    }
};
