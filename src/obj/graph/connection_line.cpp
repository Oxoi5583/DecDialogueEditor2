#include "obj/graph/connection_line.h"
#include "DecToolsBox/debug/messenger.h"
#include "obj/graph/base.h"
#include "server/object_server.h"


GraphConnectionLine::GraphConnectionLine(){
    BIND_CLASS(GraphConnectionLine);
}
GraphConnectionLine::~GraphConnectionLine(){}

void GraphConnectionLine::ready(){
    set_shape(m_line);
}
void GraphConnectionLine::pre_process(){
    m_handle_event_connect();
}
void GraphConnectionLine::process(){
    m_update_line_pos();
}
void GraphConnectionLine::post_process(){}
void GraphConnectionLine::draw(){}

void GraphConnectionLine::m_update_line_pos(){
    if(!ObjectServer::Ref()->is_id_valid(m_from_id)
    || !ObjectServer::Ref()->is_id_valid(m_to_id)){
        return;
    }
    GraphBase* fm_ptr = ObjectServer::Ref()->get_instance<GraphBase>(m_from_id);
    GraphBase* to_ptr = ObjectServer::Ref()->get_instance<GraphBase>(m_to_id);

    vec2 fm_pos = fm_ptr->get_position();
    vec2 to_pos = to_ptr->get_position();

    m_line.set_from(fm_pos);
    m_line.set_to(to_pos);
}
void GraphConnectionLine::m_handle_event_connect(){

}

OID GraphConnectionLine::get_from_id() const{
    return m_from_id;
}
OID GraphConnectionLine::get_to_id() const{
    return m_to_id;
}
void GraphConnectionLine::set_from_id(OID p_id){
    m_from_id = p_id;
}
void GraphConnectionLine::set_to_id(OID p_id){
    m_to_id = p_id;
}