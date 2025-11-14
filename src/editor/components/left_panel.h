#pragma once

#include "core/timer_server.h"
#include "editor/components/base.h"
#include "editor/space.h"
#include "obj/abstract/clickable.h"
#include "obj/graph/manager.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"

class EditorLeftPanel : public EditorComponentBase{
private:
    EditorSpace* m_space = nullptr;
    Rect2 m_shape;
    Timer* m_double_click_timer = nullptr;

    int m_current_item = -1;

    GraphManager::PanelData m_panel_data;

    void m_go_to_item(OID p_id);

    void m_update_shape();
    void m_update_inpector();
    void m_update_inpector_primary_list();
    void m_update_inpector_secondary_list(int p_parent_index);
    void m_update_inpector_other_list();
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