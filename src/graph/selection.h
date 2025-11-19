#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace glm;

class GraphSelection : public Singleton<GraphSelection>{
private:
    enum State{
        IDLE,
        DRAGGING,
    };

    State m_state = State::IDLE;
    vec2 m_start_dragging_pos;
    vec2 m_end_dragging_pos;

    Rect2 m_selection_area;

    std::vector<OID> m_selected_group_dragging_buffer;
    std::unordered_set<OID> m_selected_group_dragging_buffer_us;
    std::vector<OID> m_selected;

    void m_update_state();

    void m_block_hover_if_selecting();
    void m_update_state_TO_IDLE();
    void m_update_state_TO_DRAGGING();

    void m_process();
    void m_store_selection();

    enum Event{
        DRAG_ALL_SELECTION,
        PLACE_ALL_SELECTION,
        STORE_BUFFER,
    };
    std::queue<Event> m_events;
    void m_execute_internal_events();
    void m_execute_external_events();

    void m_drag_all_selection();
    void m_place_all_selection();

    void m_store_selection_buffer();
    void m_release_selection_buffer();

    bool m_is_group_dragging = false;
public:
    void init();
    void pre_update();
    void post_update();
    void draw();

    bool is_selecting();
    bool is_in_area(Rect2& p_rect);

    void drag_all_selection();
    void place_all_selection();

    bool is_group_dragging();

    void store_selection_buffer();
    void release_selection_buffer();

    void unselect_all();

    std::vector<OID> get_selected();
    bool is_id_in_dragging_buffer(OID& p_id);
};
