#include "system/obj/abstract/movable.h"

#include "DecToolsBox/debug/messenger.h"
#include "system/graph/camera.h"
#include "server/object_server.h"
#include <glm/ext/vector_float2.hpp>
#include <server/physics_server.h>
#include "ext/debug/messenger_ext.h"

MovableObject::MovableObject(){
    BIND_CLASS(MovableObject);
}
MovableObject::~MovableObject(){
    PhysicsServer::Ref()->erase_instance(m_shape_id);
}

bool MovableObject::is_point_intersect(vec2 p_pos){
    if(!m_shape){
        return false;
    }
    return m_shape->is_point_intersect(p_pos);
}
void MovableObject::set_position(vec2 p_pos){
    if(!m_shape){
        return;
    }
    m_shape->set_position(p_pos);
}
vec2 MovableObject::get_position(){
    if(!m_shape){
        return vec2();
    }
    return m_shape->get_position();
}

void MovableObject::ready(){
}
void MovableObject::pre_process(){
}
void MovableObject::process(){
    if(!m_shape){
        return;
    }
    m_is_on_camera = GraphCamera::Ref()->is_rect_id_on_camera(m_shape_id,m_is_full_rect_in_camera);
}
void MovableObject::post_process(){
    vec2 pos = m_shape->get_position();
    vec2 size = m_shape->get_size();
    vec2 min = pos - (size / 2.0f);
    vec2 max = pos + (size / 2.0f);
    PhysicsServer::Ref()->set_instance(m_shape_id, {min, max});
}
void MovableObject::draw(){
}

bool MovableObject::is_on_camera(){
    return m_is_on_camera;
}

void MovableObject::enable_full_rect_in_camera(){
    m_is_full_rect_in_camera = true;
}
void MovableObject::disable_full_rect_in_camera(){
    m_is_full_rect_in_camera = false;
}
ShapeId MovableObject::get_shape_id(){
    return m_shape_id;
}