#include "system/obj/graph/module_node.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "editor/components/quick_text_display.h"
#include "engine/window.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"
#include <string>

GraphModuleNode::GraphModuleNode(){
    BIND_CLASS(GraphModuleNode);
}
GraphModuleNode::~GraphModuleNode(){}

void GraphModuleNode::ready(){
    this->set_property("Module Id", "", 50);
}
void GraphModuleNode::pre_process(){}
void GraphModuleNode::process(){
    if(this->get_mouse_on_time() > TimeUnit(TimeUnit::Type::SECOND, 0.25).get_delta()){
        QuickTextDisplay::Ref()->set_text(get_property("Module Id"));
        QuickTextDisplay::Ref()->show();
    }
}
void GraphModuleNode::post_process(){}
void GraphModuleNode::draw(){}


GraphManager::NodeTypeId GraphModuleNode::get_type(){
    return GraphManager::NodeTypeId::MODULE_NODE;
}