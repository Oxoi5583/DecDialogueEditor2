#pragma once

#include <editor/components/explorer_window.h>
#include <functional>
#include "SDL3/SDL_video.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/scalar_uint_sized.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "server/project_server.h"
#include "theme/theme_loader.h"
#include <DecToolsBox/debug/messenger.h>
#include <DecToolsBox/abstract/singleton.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <queue>
#include <vector>

#include <glm/glm.hpp>

using namespace glm;

class EngineWindow : public Singleton<EngineWindow>{
private:
    SDL_WindowFlags m_sdl_window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY| SDL_WINDOW_BORDERLESS;
    SDL_Window* m_sdl_window = nullptr;
    SDL_Renderer* m_sdl_renderer = nullptr;
    SDL_GLContext m_sdl_gl_context;

    const char* m_main_window_title;
    const char* m_glsl_version = "#version 420 core";

    unsigned long long m_frame = 0;

    vec4 m_clear_color = ThemeLoader::Ref()->get_color("BrandColour");

    bool m_is_running = true;


    Uint64 m_this_total_time = 0;
    Uint64 m_last_total_time = 0;
    Uint64 m_delta = 0;

    const vec2 m_min_window_size = {600, 300};

    void m_init_check_step(const int& p_step);

    void m_init_sdl_Engine();
    void m_init_sdl_set_gl_attribute();
    void m_init_create_sdl_window();
    void m_init_create_sdl_renderer();
    void m_init_create_sdl_gl_context();
    void m_init_imgui_Engine();
    void m_init_sdl_show_window();
    
    void m_job_update_screen_mouse_pos();
    void m_job_update_window_dragging();
    void m_job_update_delta();
    void m_job_set_delay_if_minimized();
    void m_job_gl_clear();
    void m_job_imgui_new_frame();
    void m_job_event_handle();
    void m_job_resizer_handle();

    void m_job_imgui_render();

    int m_init_step = 0;
    bool m_is_init_done = false;

    int m_width;
    int m_height;


    double proportion_x;
    double proportion_y = 0.5f;

    bool m_is_window_dragged = false;
    bool m_is_first_screen_mouse_position = true;
    vec2 m_window_position;
    vec2 m_window_dragging_offset;
    vec2 m_screen_mouse_last_position;
    vec2 m_screen_mouse_motion;
    vec2 m_screen_mouse_position;

    enum class Event{
        START_DRAG,
        END_DRAG,
        CLOSE_WINDOW,
        MAXIMIZE,
        MINIMIZE,
        RESTORE,
    };
    std::queue<Event> m_events;

    vec2 m_pos_buffer;
    vec2 m_size_buffer;
    void m_store_buffer();
    void m_apply_buffer();
public:
    EngineWindow()
    :m_main_window_title("DecDialogueEditor (DEV)"){}
    ~EngineWindow(){}

    void init();

    void shutdown();

    void set_clear_color(vec4 p_color);
    vec4 get_clear_color();
    Uint64 get_total_time();
    Uint64 get_delta();

    vec2 get_display_size();

    void delay(uint32 p_delay);
    bool is_running();
    void close();

    void begin();
    void end();
    vec2 get_window_size();
    vec2 get_window_position();
    void set_window_position(vec2 p_pos);
    void set_window_AABB(vec2 p_left_top, vec2 p_right_down);
    void window_follow_mouse();

    void start_dragging();
    void stop_dragging();

    bool is_maximized();
    bool is_minimized();
    void maximize();
    void minimize();
    void restore();

    void focus();

    void after_restore();
    unsigned int get_window_id();

    void move_left_top(vec2 p_pos);
    void move_left_down(vec2 p_pos);
    void move_right_top(vec2 p_pos);
    void move_right_down(vec2 p_pos);
    void move_left(double p_x);
    void move_right(double p_x);
    void move_top(double p_y);
    void move_down(double p_y);

    void refresh();

    unsigned long long get_frame();

    ExplorerWindow* create_explorer_window();
};
