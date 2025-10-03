#pragma once

#include <DecToolsBox/abstract/singleton.h>
#include <SDL3/SDL.h>

class CoreLoop : public Singleton<CoreLoop>{
private:
    bool m_is_running = true;

    SDL_Event m_event;
    void m_job_event_handling();
public:
    void start();
    void stop();

    bool poll();
};