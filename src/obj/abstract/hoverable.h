#pragma once

#include "obj/abstract/movable.h"
#include "server/object_base.h"
#include "struct/shape/base.h"
#include <memory>

class HoverableObject : public MovableObject {
public:
    HoverableObject();
    ~HoverableObject();


    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    bool was_hovered();

    bool is_changing_cursor();
    void disable_cursor_change();
    void enable_cursor_change();

    enum class Type{
        SCREEN,
        WORLD,
    };

    Type get_hovering_type();
    void set_hovering_type(Type p_type);
private:
    bool m_was_hovered = false;
    bool m_check_hovering();

    bool m_changed_cursor = true;
    
    Type m_type = Type::WORLD;

    vec2 get_mouse_pos();
};