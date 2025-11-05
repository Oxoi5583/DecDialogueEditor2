#pragma once

#include "obj/abstract/hoverable.h"
#include "server/object_base.h"

class ClickableObject : public HoverableObject {
private:
    bool m_was_just_clicked = false;
    bool m_was_clicked = false;
    bool m_was_just_released = false;

    bool m_changed_cursor = true;
public:
    ClickableObject();
    ~ClickableObject();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
    
    bool was_just_clicked();
    bool was_clicked();
    bool was_just_released();

    bool is_changing_cursor();
    void disable_cursor_change();
    void enable_cursor_change();
};