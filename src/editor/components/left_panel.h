#pragma once

#include "core/timer_server.h"
#include "editor/components/base.h"
#include "editor/space.h"
#include "system/obj/abstract/clickable.h"
#include "system/obj/graph/manager.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"

class EditorLeftPanel : public EditorComponentBase{
private:
    EditorSpace* m_space = nullptr;
    Rect2 m_shape;
    Timer* m_double_click_timer = nullptr;

    bool m_is_displaying = false;
    Rect2 m_window_rect;

    const float m_hint_width = 5.0f;

    int m_current_item = -1;

    GraphManager::PanelData m_panel_data;

    void m_go_to_item(OID p_id);

    void m_update_penel_properties();
    void m_update_shape();
    void m_update_inpector();
    void m_update_inpector_primary_list();
    void m_update_inpector_secondary_list(int p_parent_index);
    void m_update_inpector_other_list();

    void m_emit_event();

    int m_index = 0;

    void m_store_last_click_id(OID p_id);
    bool m_is_select_range = false;
    std::vector<OID> m_iterated_ids;
    OID m_last_1_click_id;
    OID m_last_2_click_id;

    void m_update_item_status(OID& p_id);
    void m_update_range_select();

    void m_refresh_buffers();
    void m_refresh_panel_data();
public:
    EditorLeftPanel();
    ~EditorLeftPanel();
    
    void ui_init(EditorSpace* p_space) override;

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};