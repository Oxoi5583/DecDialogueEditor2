#pragma once

#include "SDL3/SDL_stdinc.h"
#include "imgui/imgui.h"
#include "theme/theme_loader.h"
#include <DecToolsBox/abstract/singleton.h>
#include <SDL3/SDL.h>

class CoreLoop : public Singleton<CoreLoop>{
private:
    bool m_is_running = true;

    SDL_Event m_event;
    void m_job_event_handling();
    void m_job_set_sdl_delay();
    void m_job_update_delta();

    ImVec4 m_clear_color = ThemeLoader::Ref()->get_color("BrandColour");

    Uint64 m_this_total_time = 0;
    Uint64 m_last_total_time = 0;
    Uint64 m_delta = 0;
public:
    void start();
    void stop();

    bool poll();

    void pre_process();
    void process();
    void post_process();

    void set_clear_color(ImVec4 p_color);
    ImVec4 get_clear_color();

    void new_frame();
    void render();

    Uint64 get_total_time();
    Uint64 get_delta();
};