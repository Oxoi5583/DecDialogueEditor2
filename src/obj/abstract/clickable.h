#pragma once

#include "obj/abstract/hoverable.h"
#include "server/object_base.h"

class ClickableObject : public HoverableObject {
private:
    bool m_was_just_clicked = false;
    bool m_was_clicked = false;
    bool m_was_just_released = false;

    void m_emit_event();
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

};