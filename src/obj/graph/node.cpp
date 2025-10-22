#include "obj/graph/node.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "server/object_server.h"

GraphNode::GraphNode(){
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
    this->set_shape(m_rect);
    m_test_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::SECOND, 1.0f), false);
    m_test_timer->stop();
}
void GraphNode::pre_process(){}
void GraphNode::process(){}
void GraphNode::post_process(){}
void GraphNode::draw(){
    if(this->was_just_clicked()){
        m_test_timer->finish();
        m_test_timer->reset_cycle();
        m_test_timer->start();
    }
    if(m_test_timer->timeout_and_reset_in_cycle(5, TimeUnit(TimeUnit::Type::SECOND, 2.0f))){
        EngineRenderer::Ref()->draw_rect(m_rect, vec4(1.0f,0.0f,0.0f,1.0f), 0);
    }else{
        EngineRenderer::Ref()->draw_rect(m_rect, vec4(0.0f,0.0f,0.0f,1.0f), 0);
    }
}