#include "system/graph/background.h"
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

    EventMouseHoverOnWorld event;
    event.pos = MouseServer::Ref()->get_mouse_world_position();
    event.screen_pos = MouseServer::Ref()->get_mouse_screen_position();
    EventServer::Ref()->emit(event);
    
    if(MouseServer::Ref()->is_just_clicked(MouseButton::MIDDLE)){
        EventMouseJustClickedOnWorld event2;
        event2.pos = MouseServer::Ref()->get_mouse_world_position();
        event2.screen_pos = MouseServer::Ref()->get_mouse_screen_position();
        event2.button = (int)MouseButton::MIDDLE;
        EventServer::Ref()->emit(event2);
    }

    if(MouseServer::Ref()->is_just_clicked(MouseButton::LEFT)){
        EventMouseJustClickedOnWorld event3;
        event3.pos = MouseServer::Ref()->get_mouse_world_position();
        event3.screen_pos = MouseServer::Ref()->get_mouse_screen_position();
        event3.button = (int)MouseButton::LEFT;
        EventServer::Ref()->emit(event3);
    }
}
void GraphBackground::init(){

}
void GraphBackground::update(){
    m_emit_event_if_hit_background();
}