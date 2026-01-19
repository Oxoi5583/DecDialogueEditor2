#include "system/obj/graph/node.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "editor/components/quick_text_display.h"
#include "engine/window.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"

GraphNode::GraphNode(){
    BIND_CLASS(GraphNode);
}
GraphNode::~GraphNode(){}

void GraphNode::ready(){
    this->add_property("Content", "", 1000);
}
void GraphNode::pre_process(){}
void GraphNode::process(){
    if(this->get_mouse_on_time() > TimeUnit(TimeUnit::Type::SECOND, 0.25).get_delta()){
        QuickTextDisplay::Ref()->set_text(get_property("Content"));
        QuickTextDisplay::Ref()->show();
    }
}
void GraphNode::post_process(){}
void GraphNode::draw(){}


GraphManager::NodeType GraphNode::get_type(){
    return GraphManager::NodeType::NODE;
}
