#pragma once

#include "system/obj/abstract/clickable.h"
#include "system/obj/abstract/dragable.h"
#include "server/object_base.h"

class SelectableObject : public DragableObject {
private:
    enum State{
        IDLE,
        READY,
        SELECTED,
        RESTORE,
    };
    State m_state = State::IDLE;


    void m_select_if_clicked();
    void m_select_if_in_select_area();

    void m_unselect_if_lshift_clicked();
    void m_unselect_if_other_clicked();
    void m_unselect_if_world_clicked();
    void m_unselect_if_not_in_select_area();

    void m_emit_event();

   void m_update_state();
public:
    SelectableObject();
    ~SelectableObject();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    void select();
    void unselect();
    bool is_selected();
};