#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/graph/base.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace glm;

class GraphConnection : public Singleton<GraphConnection>{
private:
    enum State{
        NUL,
        START_CONNECT,
        SEARCH_CONNECT,
        PLACED_CONNECT,
    };
    State m_state = State::NUL;

    OID m_start_id;
    OID m_end_id;

    std::map<OID, std::set<OID>> m_connections;

    bool m_condition_nul_to_start();
    bool m_condition_start_to_search();
    bool m_condition_search_to_placed();
    bool m_condition_search_to_nul();
    bool m_condition_placed_to_nul();

    void m_update_state();

    void m_emit_event();
    void m_create_connection_obj();
    void m_handle_event();

    std::vector<OID> m_on_screen_ids;
    void m_refresh_screen_ids();
    std::unordered_set<OID> m_connectables;
    void m_refresh_connectables();

    bool m_test_connection__target_not_self(GraphBase* p_fm, GraphBase* p_to);
    bool m_test_connection__target_not_entry(GraphBase* p_fm, GraphBase* p_to);
    bool m_test_connection__target_not_connected(GraphBase* p_fm, GraphBase* p_to);
    bool m_test_connection__to_repeater_not_have_parent(GraphBase* p_fm, GraphBase* p_to);
    bool m_test_connection__fm_repeater_not_have_children(GraphBase* p_fm, GraphBase* p_to);
public:
    void init();
    void pre_update();
    void post_update();
    void draw();

    bool is_connecting();

    std::set<OID> get_connection(OID p_id);

    bool test_connection(OID p_fm_id, OID p_to_id);
};
