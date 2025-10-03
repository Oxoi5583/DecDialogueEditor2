#include "core/loop.h"

void CoreLoop::start(){
    m_is_running = true;
}
void CoreLoop::stop(){
    m_is_running = false;
}

bool CoreLoop::poll(){
    m_job_event_handling();
    return m_is_running;
}


void CoreLoop::m_job_event_handling(){
    while (SDL_PollEvent(&m_event)) {
        if (m_event.type == SDL_EVENT_QUIT) {
            this->stop();
        }
    }
}