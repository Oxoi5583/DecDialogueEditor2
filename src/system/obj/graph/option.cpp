#include "system/obj/graph/option.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"

GraphOption::GraphOption(){
    BIND_CLASS(GraphOption);
}
GraphOption::~GraphOption(){}

void GraphOption::ready(){
    this->set_property("Content", "", 1000);
}
void GraphOption::pre_process(){}
void GraphOption::process(){}
void GraphOption::post_process(){}
void GraphOption::draw(){}

GraphManager::NodeTypeId GraphOption::get_type(){
    return GraphManager::NodeTypeId::OPTION;
}
