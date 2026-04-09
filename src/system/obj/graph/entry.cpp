#include "system/obj/graph/entry.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"

GraphEntry::GraphEntry(){
    BIND_CLASS(GraphEntry);
}
GraphEntry::~GraphEntry(){}

void GraphEntry::ready(){}
void GraphEntry::pre_process(){}
void GraphEntry::process(){}
void GraphEntry::post_process(){}
void GraphEntry::draw(){}

GraphManager::NodeTypeId GraphEntry::get_type(){
    return GraphManager::NodeTypeId::ENTRY;
}