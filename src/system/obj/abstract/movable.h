#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include "struct/shape/base.h"
#include <memory>
#include <server/physics_server.h>

class MovableObject : public ObjectBase {
private:
    std::unique_ptr<ShapeBase> m_shape = nullptr;
    bool m_is_on_camera = false;
    bool m_is_full_rect_in_camera = false; 

    vec2 m_points[4] = {vec2(),vec2(),vec2(),vec2()};

    vec2 last_frame_shape_min = {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()};
    vec2 last_frame_shape_max = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};

    ShapeId m_shape_id = PhysicsServer::Ref()->next_shape_id();
public:
    MovableObject();
    ~MovableObject();

    template<typename T>
    void set_shape(T p_shape){
        static_assert( std::is_base_of<ShapeBase, T>::value, "Shape have to be inherited from class ShapeBase." );
        m_shape = std::make_unique<T>(p_shape);
    }
    template<typename T>
    T& get_shape() const{
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

    bool is_on_camera();
    void enable_full_rect_in_camera();
    void disable_full_rect_in_camera();

    void update_physics_server_instance();

    ShapeId get_shape_id();
};