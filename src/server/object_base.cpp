#include "server/object_base.h"
#include "server/object_server.h"

OID ObjectBase::m_next_id = 100;

OID ObjectBase::fetch_id(){
    OID return_id = m_next_id;
    m_next_id++;
    return return_id;
}

OID ObjectBase::get_id() const{
    return m_id;
}

bool ObjectBase::is_alive() const{
    return m_is_alive;
}

void ObjectBase::queue_free(){
    m_is_alive = false;
}

bool ObjectBase::is_ready() const{
    return m_is_ready;
}

void ObjectBase::set_layer(int p_layer){
    m_layer = p_layer;
}
int ObjectBase::get_layer(){
    return m_layer;
}

bool ObjectBase::is_freeze() const{
    return m_is_freeze;
}

void ObjectBase::set_freeze(bool p_value){
    m_is_freeze = p_value;
}