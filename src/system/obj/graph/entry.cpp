#include "system/obj/graph/entry.h"
#include "DecToolsBox/debug/messenger.h"
#include "editor/components/quick_text_display.h"
#include "server/timer_server.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"

GraphEntry::GraphEntry(){
    BIND_CLASS(GraphEntry);
}
GraphEntry::~GraphEntry(){}

void GraphEntry::ready(){
    this->set_init_property("Hint", "", 150);
}
void GraphEntry::pre_process(){}
void GraphEntry::process(){
    if(this->get_mouse_on_time() > TimeUnit(TimeUnit::Type::SECOND, 0.25).get_delta()){
        QuickTextDisplay::Ref()->set_text(get_property("Hint"));
        QuickTextDisplay::Ref()->show();
    }
}
void GraphEntry::post_process(){}
void GraphEntry::draw(){}

GraphManager::NodeTypeId GraphEntry::get_type(){
    return GraphManager::NodeTypeId::ENTRY;
}