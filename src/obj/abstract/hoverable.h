#pragma once

#include "server/object_base.h"
#include "struct/shape/base.h"
#include <memory>

class HoverableObject : public ObjectBase {
private:
    std::unique_ptr<ShapeBase> m_shape = std::make_unique<ShapeBase>();

    bool m_was_hovered = false;
    bool m_check_hovering();
public:
    HoverableObject();
    ~HoverableObject();

    template<typename T>
    void set_shape(T& p_shape){
        static_assert( std::is_base_of<ShapeBase, T>::value, "Shape have to be inherited from class ShapeBase." );
        m_shape = std::make_unique<T>(p_shape);
    }

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    bool was_hovered();
};