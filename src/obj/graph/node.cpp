#include "obj/graph/node.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"

GraphNode::GraphNode(){
    BIND_CLASS(GraphNode);
}
GraphNode::~GraphNode(){}

void GraphNode::ready(){}
void GraphNode::pre_process(){}
void GraphNode::process(){}
void GraphNode::post_process(){}
void GraphNode::draw(){}

GraphManager::NodeType GraphNode::get_type(){
    return GraphManager::NodeType::NODE;
}
