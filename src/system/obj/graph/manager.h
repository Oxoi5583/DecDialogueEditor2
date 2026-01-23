#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "core/timer_server.h"
#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace glm;

class GraphBase;

class GraphManager : public Singleton<GraphManager> {
public:
    enum NodeType{
        BASE,
        ENTRY,
        NODE,
        OPTION,
        REPEATER,
    };

    struct NodeInfo{
        OID id;
        NodeType type;
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

    std::unordered_map<std::string,OID> m_exists_names;
private:
    const double m_data_refresh_second = 1.2f;
    Timer* m_data_refresh_timer;

    std::vector<OID> m_all_node_ids;
    std::vector<OID> m_entry_node_ids;
    std::vector<OID> m_non_entry_node_ids;
    std::unordered_map<OID, NodeInfo> m_infos;

    PanelData m_panel_data;

    void m_poll_spawn_event();
    void m_clear_garbage();
    void m_refetch_node_data();
    void m_regenerate_panel_data();
    void m_data_refresh();

    NodeInfo m_create_info(GraphBase* m_ptr);
    std::vector<OID> m_get_all_children(OID p_parent_id, 
                                        bool p_is_self_contain = true,
                                        std::unordered_set<OID> p_ancestors = {});

    void m_spawn_entry(vec2 p_pos);
    void m_spawn_node(vec2 p_pos);
    void m_spawn_option(vec2 p_pos);
    void m_spawn_repeater(vec2 p_pos);
public:
    GraphManager();
    ~GraphManager();

    void init();
    void update();

    bool is_name_duplicated(OID p_id, std::string p_name);
    std::string new_name_if_duplicated(OID p_id, std::string p_name);
    std::string get_default_name(NodeType p_type);

    void notify_name_removed(std::string p_name);
    void notify_name_added(OID p_id, std::string p_name);

    PanelData get_panel_data();
};