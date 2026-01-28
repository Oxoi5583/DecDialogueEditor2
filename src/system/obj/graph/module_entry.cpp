#include "system/obj/graph/module_entry.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"

GraphModuleEntry::GraphModuleEntry(){
    BIND_CLASS(GraphModuleEntry);
}
GraphModuleEntry::~GraphModuleEntry(){}

void GraphModuleEntry::ready(){}
void GraphModuleEntry::pre_process(){}
void GraphModuleEntry::process(){}
void GraphModuleEntry::post_process(){}
void GraphModuleEntry::draw(){}

GraphManager::NodeType GraphModuleEntry::get_type(){
    return GraphManager::NodeType::MODULE_ENTRY;
}

std::string GraphModuleEntry::get_type_name(){
    return "Module Entry";
}