#pragma once

#include <functional>
#include "SDL3/SDL_video.h"
#include <DecToolsBox/debug/messenger.h>
#include <DecToolsBox/abstract/singleton.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <vector>

class CoreInitializer : public Singleton<CoreInitializer>{
private:
    SDL_WindowFlags m_sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    SDL_GLContext m_sdl_gl_context;

    const char* m_main_window_title;

    void m_init_check_step(const int& p_step);

    void m_init_sdl_core();
    void m_init_sdl_set_gl_attribute();
    void m_init_create_sdl_window();
    void m_init_create_sdl_renderer();
    void m_init_create_sdl_gl_context();
    void m_init_imgui_core();
    void m_init_sdl_show_window();

    int m_init_step = 0;
    bool m_is_init_done = false;
public:
    CoreInitializer()
    :m_main_window_title("DecDialogueEditor (DEV)"){
        init_all();
    }
    ~CoreInitializer(){}

    void init_all();

    void destory_all();

    SDL_Renderer* get_sdl_renderer();
    SDL_Window* get_sdl_window();
    SDL_GLContext get_sdl_glcontext();
    ImGuiIO& get_imgui_io();
    ImGuiStyle& get_imgui_style();
};
