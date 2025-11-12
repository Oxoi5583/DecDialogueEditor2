#include "obj/graph/node.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"


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
    if(!is_on_camera()){
        return;
    }

    if(this->was_clicked()){
        EngineRenderer::Ref()->draw_rect(m_rect, vec4(0.0f,0.0f,0.0f,1.0f), 1);
    }else{
        EngineRenderer::Ref()->draw_rect(m_rect, vec4(0.0f,0.0f,0.0f,1.0f), 0);
    }

    if(this->is_selected()){
        const double width = 3;

        vec2 lt = m_rect.get_left_top();
        vec2 rt = m_rect.get_right_top();
        vec2 rd = m_rect.get_right_down();
        vec2 ld = m_rect.get_left_down();

        vec4 border_color = ThemeLoader::Ref()->get_color("SelectableHighlightColour");


        EngineRenderer::Ref()->draw_line(lt - vec2(width / 2.0f, 0.0f), rt + vec2(width / 2.0f, 0.0f), border_color, width);
        EngineRenderer::Ref()->draw_line(rt - vec2( 0.0f, width / 2.0f), rd + vec2( 0.0f, width / 2.0f), border_color, width);
        EngineRenderer::Ref()->draw_line(rd + vec2(width / 2.0f, 0.0f), ld - vec2(width / 2.0f, 0.0f), border_color, width);
        EngineRenderer::Ref()->draw_line(ld + vec2( 0.0f, width / 2.0f), lt - vec2( 0.0f, width / 2.0f), border_color, width);
    }
}