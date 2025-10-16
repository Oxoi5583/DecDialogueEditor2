#pragma once

#include <functional>
#include "SDL3/SDL_video.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/scalar_uint_sized.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "theme/theme_loader.h"
#include <DecToolsBox/debug/messenger.h>
#include <DecToolsBox/abstract/singleton.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <vector>

#include <glm/glm.hpp>

using namespace glm;

class EngineWindow : public Singleton<EngineWindow>{
private:
    SDL_WindowFlags m_sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    SDL_GLContext m_sdl_gl_context;

    const char* m_main_window_title;
    const char* m_glsl_version = "#version 420 core";

    vec4 m_clear_color = ThemeLoader::Ref()->get_color("BrandColour");

    bool m_is_running = true;


    Uint64 m_this_total_time = 0;
    Uint64 m_last_total_time = 0;
    Uint64 m_delta = 0;

    void m_init_check_step(const int& p_step);

    void m_init_sdl_Engine();
    void m_init_sdl_set_gl_attribute();
    void m_init_create_sdl_window();
    void m_init_create_sdl_renderer();
    void m_init_create_sdl_gl_context();
    void m_init_imgui_Engine();
    void m_init_sdl_show_window();
    
    void m_job_update_delta();
    void m_job_set_delay_if_minimized();
    void m_job_gl_clear();
    void m_job_imgui_new_frame();
    void m_job_close_event_handle();

    void m_job_imgui_render();

    int m_init_step = 0;
    bool m_is_init_done = false;

    int m_width;
    int m_height;
public:
    EngineWindow()
    :m_main_window_title("DecDialogueEditor (DEV)"){}
    ~EngineWindow(){}

    void init();

    void destory_all();

    void set_clear_color(vec4 p_color);
    vec4 get_clear_color();
    Uint64 get_total_time();
    Uint64 get_delta();

    vec2 get_display_size();
    bool is_minimized();

    void delay(uint32 p_delay);
    bool is_running();
    void close();

    void begin();
    void end();

    vec2 get_window_size();
};
