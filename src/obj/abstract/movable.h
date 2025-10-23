#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include "struct/shape/base.h"
#include <memory>

class MovableObject : public ObjectBase {
private:
    std::unique_ptr<ShapeBase> m_shape = std::make_unique<ShapeBase>();

public:
    MovableObject();
    ~MovableObject();


    template<typename T>
    void set_shape(T p_shape){
        static_assert( std::is_base_of<ShapeBase, T>::value, "Shape have to be inherited from class ShapeBase." );
        m_shape = std::make_unique<T>(p_shape);
    }
    template<typename T>
    T& get_shape(){
        static_assert( std::is_base_of<ShapeBase, T>::value, "Shape have to be inherited from class ShapeBase." );
        return static_cast<T&>(*m_shape.get());
    }

    bool is_point_intersect(vec2 p_pos);
    void set_position(vec2 p_pos);
    vec2 get_position();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    
};