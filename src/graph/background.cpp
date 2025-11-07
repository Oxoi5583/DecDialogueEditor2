#include "graph/background.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/window.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"


void GraphBackground::m_emit_event_if_hit_background(){
    if(EventServer::Ref()->has<EventMouseHoverObj>()){
        return;
    }

    if(!MouseServer::Ref()->is_mouse_in_window()){
        return;
    }

    if(MouseServer::Ref()->is_just_clicked(MouseButton::MIDDLE)){
        EventMouseJustClickedOnWorld event;
        event.pos = MouseServer::Ref()->get_mouse_screen_position();
        event.button = (int)MouseButton::MIDDLE;
        EventServer::Ref()->emit(event);
    }
}
void GraphBackground::init(){

}
void GraphBackground::update(){
    m_emit_event_if_hit_background();
}