#include "obj/graph/node.h"
#include "server/object_server.h"

GraphNode::GraphNode(){
    BIND_CLASS(GraphNode);
}
GraphNode::~GraphNode(){

}

vec2 GraphNode::get_position() const{
    return m_rect.get_position();
}
vec2 GraphNode::get_size() const{
    return m_rect.get_size();
}
void GraphNode::set_position(vec2& p_position){
    m_rect.set_position(p_position);
}
void GraphNode::set_size(vec2& p_size){
    m_rect.set_size(p_size);
}

bool GraphNode::is_point_intersect(vec2& p_point){
    return m_rect.is_point_intersect(p_point);
}

void GraphNode::draw(){
    EngineRenderer::Ref()->draw_rect(m_rect, vec4(0.0f,0.0f,0.0f,1.0f), 0);
}