#include "obj/graph/node.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "server/object_server.h"


Rect2& GraphNode::m_init_shape(){
    Rect2 shape = {vec2(0.0f, 0.0f), vec2(50.0f, 50.0f)};
    this->set_shape(shape);
    return this->get_shape<Rect2>();
}

GraphNode::GraphNode()
: m_rect(m_init_shape()){
    BIND_CLASS(GraphNode);
}
GraphNode::~GraphNode(){
    m_test_timer->queue_free();
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

void GraphNode::ready(){

    m_test_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::SECOND, 1.0f), false);
    m_test_timer->stop();
}
void GraphNode::pre_process(){}
void GraphNode::process(){}
void GraphNode::post_process(){}
void GraphNode::draw(){
    if(this->was_clicked()){
        EngineRenderer::Ref()->draw_rect(m_rect, vec4(0.0f,0.0f,0.0f,1.0f), 1);
    }else{
        EngineRenderer::Ref()->draw_rect(m_rect, vec4(0.0f,0.0f,0.0f,1.0f), 0);
    }
}