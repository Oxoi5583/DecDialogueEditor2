#pragma once

#include "SDL3/SDL_video.h"
#include <DecToolsBox/debug/messenger.h>
#include <DecToolsBox/abstract/singleton.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>

class CoreInitializer : public Singleton<CoreInitializer>{
private:
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context;

    const char* m_title = "DecDialogueEditor (DEV)";

    int m_init_step = 0;
    bool m_is_init_done = false;
public:
    void init_step0_sdl_core();
    void init_step1_sdl_window();
    void init_step2_sdl_renderer();
    void init_step3_sdl_gl_context();
    void init_all();

    void destory_all();

    SDL_Renderer* get_renderer();
    SDL_Window* get_window();
    SDL_GLContext get_glcontext();
};
