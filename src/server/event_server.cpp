#include "server/event_server.h"
#include <server/events.h>

void EventServer::flush(){
    m_events_buffer = std::move(m_events);
    std::set<std::type_index>().swap(m_block_types);
}

void EventServer::emit_locked_all(){
    {
        EventMouseHoverObj evt;
        evt.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
        evt.obj_id = -1;
        EventServer::Ref()->emit(evt);
    }
    {
        EventLockedAll evt;
        EventServer::Ref()->emit(evt);
    }

}