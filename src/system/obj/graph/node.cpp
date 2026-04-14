#include "system/obj/graph/node.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "editor/components/quick_text_display.h"
#include "engine/window.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"
#include <string>

GraphNode::GraphNode(){
    BIND_CLASS(GraphNode);
}
GraphNode::~GraphNode(){}

void GraphNode::ready(){
    this->set_property("Content", "", 150);
}
void GraphNode::pre_process(){}
void GraphNode::process(){
}
void GraphNode::post_process(){}
void GraphNode::draw(){}


GraphManager::NodeTypeId GraphNode::get_type(){
    return GraphManager::NodeTypeId::NODE;
}