#include "obj/abstract/movable.h"

#include "server/object_server.h"


MovableObject::MovableObject(){
    BIND_CLASS(MovableObject);
}
MovableObject::~MovableObject(){

}

bool MovableObject::is_point_intersect(vec2 p_pos){
    return m_shape->is_point_intersect(p_pos);
}
void MovableObject::set_position(vec2 p_pos){
    m_shape->set_position(p_pos);
}
vec2 MovableObject::get_position(){
    return m_shape->get_position();
}

void MovableObject::ready(){
}
void MovableObject::pre_process(){
}
void MovableObject::process(){
}
void MovableObject::post_process(){
}
void MovableObject::draw(){
}