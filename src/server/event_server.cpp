#include "server/event_server.h"

void EventServer::flush(){
    m_events_buffer = std::move(m_events);
}