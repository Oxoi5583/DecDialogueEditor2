#include "engine/window.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_render.h"
#include "editor/components/menu_bar.h"
#include "editor/layout.h"
#include "ext/debug/messenger_ext.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_video.h"
#include "engine/input_hub.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_int3.hpp"
#include <editor/components/explorer_window.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "engine/input_hub.h"
#include "server/event_server.h"
#include "server/events.h"
#include "struct/shape/rect2.h"
#include <server/object_server.h>
#include <vector>

#define ENGINE_INIT_STEP __COUNTER__
#define ENGINE_INIT_BIND(FUNC) std::bind(&EngineWindow::FUNC, this)

void EngineWindow::init(){
    if(!m_is_init_done){
        const std::vector<std::function<void()>> all_init_steps = {
            ENGINE_INIT_BIND(m_init_sdl_Engine),
            ENGINE_INIT_BIND(m_init_sdl_set_gl_attribute),
            ENGINE_INIT_BIND(m_init_create_sdl_window),
            ENGINE_INIT_BIND(m_init_create_sdl_renderer),
            ENGINE_INIT_BIND(m_init_create_sdl_gl_context),
            ENGINE_INIT_BIND(m_init_imgui_Engine),
            ENGINE_INIT_BIND(m_init_sdl_show_window),
        };

        for(auto& step_func : all_init_steps){
            step_func();
        }

        m_store_buffer();
    }
}

void EngineWindow::m_init_check_step(const int& p_step){
    if(m_init_step != p_step){
        ERROR_MSG("SDL_Init Error: " << "Incorrect Step. Current step is " << m_init_step << " and it should be " << p_step << ".");
        exit(-1);
    }
    m_init_step++;
}

void EngineWindow::m_init_sdl_Engine(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        ERROR_MSG("SDL_Init Error: " << SDL_GetError());
        exit(-1);
    }
}
void EngineWindow::m_init_sdl_set_gl_attribute(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
}
void EngineWindow::m_init_create_sdl_window(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    m_sdl_window = SDL_CreateWindow(m_main_window_title,800, 600, m_sdl_window_flags);
    if (!m_sdl_window) {
        ERROR_MSG("SDL_CreateWindow Error: " << SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
}
void EngineWindow::m_init_create_sdl_renderer(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    m_frame++;

    /*
    m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, NULL);
    if (!m_sdl_renderer) {
        ERROR_MSG("SDL_CreateRenderer Error: " << SDL_GetError());
        SDL_DestroyWindow(m_sdl_window);
        SDL_Quit();
        exit(-1);
    }
    */
}
void EngineWindow::m_init_create_sdl_gl_context(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    m_sdl_gl_context = SDL_GL_CreateContext(m_sdl_window);
    if (!m_sdl_gl_context) {
        ERROR_MSG("SDL_GL_CreateContext Error: " << SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        exit(-1);
    }
    SDL_GL_MakeCurrent(m_sdl_window, m_sdl_gl_context);
    SDL_GL_SetSwapInterval(1); 
}
void EngineWindow::m_init_imgui_Engine(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& m_imgui_io = ImGui::GetIO();
    ImGuiStyle& m_imgui_style = ImGui::GetStyle();
    (void)m_imgui_io;
    m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    m_imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    m_imgui_style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    m_imgui_style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    ImGui_ImplSDL3_InitForOpenGL(m_sdl_window, m_sdl_gl_context);
    ImGui_ImplOpenGL3_Init(m_glsl_version);
}
void EngineWindow::m_init_sdl_show_window(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    SDL_SetWindowPosition(m_sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(m_sdl_window);

    m_is_init_done = true;
}
void EngineWindow::shutdown(){
    SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();
}


void EngineWindow::m_job_gl_clear(){
    vec2 window_size = this->get_window_size();

    glViewport(0, 0, (int)window_size.x, (int)window_size.y);
    glClearColor(m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}
void EngineWindow::m_job_imgui_new_frame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}
void EngineWindow::m_job_imgui_render(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_sdl_window);
}


void EngineWindow::set_clear_color(vec4 p_color){
    m_clear_color = {p_color.x, p_color.y, p_color.z, p_color.w};
}
vec4 EngineWindow::get_clear_color(){
    return {m_clear_color.x, m_clear_color.y, m_clear_color.z, m_clear_color.w};
}

vec2 EngineWindow::get_display_size(){
    ImVec2 ret = ImGui::GetIO().DisplaySize;
    return {ret.x, ret.y};
}

void EngineWindow::delay(uint32 p_delay){
    SDL_Delay(10);
}

void EngineWindow::m_job_update_screen_mouse_pos(){
    float screen_x, screen_y;
    SDL_GetGlobalMouseState(&screen_x, &screen_y);
    m_screen_mouse_last_position = m_screen_mouse_position;
    m_screen_mouse_position = {screen_x, screen_y};
    if(!m_is_first_screen_mouse_position){
        m_screen_mouse_motion = m_screen_mouse_position - m_screen_mouse_last_position;
    }
    m_is_first_screen_mouse_position = false;

    int window_pos_x, window_pos_y;
    SDL_GetWindowPosition(m_sdl_window, &window_pos_x, &window_pos_y);
    m_window_position = {window_pos_x, window_pos_y};
}
void EngineWindow::m_job_update_window_dragging(){
    if(m_is_window_dragged){
        window_follow_mouse();
    }
}
void EngineWindow::m_job_set_delay_if_minimized(){
    if(this->is_minimized()){
        this->delay(10);
    }
}
void EngineWindow::m_job_event_handle(){
    while (!m_events.empty()) {
        switch (m_events.front()) {
            case Event::START_DRAG:{
                m_is_window_dragged = true;
                m_window_dragging_offset = m_screen_mouse_position - m_window_position;
                break;
            }
            case Event::END_DRAG:{
                m_is_window_dragged = false;
                break;
            }
            case Event::CLOSE_WINDOW:{
                m_is_running = false;
                break;
            }
            case Event::MAXIMIZE:{
                if(is_maximized()){
                    return;
                }

                m_store_buffer();

                SDL_MaximizeWindow(m_sdl_window);

                this->stop_dragging();
                m_job_event_handle();

                break;
            }
            case Event::MINIMIZE:{
                if(is_minimized()){
                    return;
                }

                //m_store_buffer();

                SDL_MinimizeWindow(m_sdl_window);

                this->stop_dragging();
                m_job_event_handle();
                
                break;
            }
            case Event::RESTORE:{
                SDL_HideWindow(m_sdl_window);
                SDL_RestoreWindow(m_sdl_window);
                SDL_ShowWindow(m_sdl_window);
                break;
            }
        }
        m_events.pop();
    }
}

void EngineWindow::close(){
    m_events.emplace(Event::CLOSE_WINDOW);
}

void EngineWindow::begin(){
    m_job_resizer_handle();
    m_job_update_screen_mouse_pos();
    m_job_update_delta();
    m_job_set_delay_if_minimized();
    m_job_gl_clear();
    m_job_imgui_new_frame();
    m_job_event_handle();
    m_job_update_window_dragging();
}
void EngineWindow::end(){
    EngineWindow::Ref()->m_job_imgui_render();
}

bool EngineWindow::is_running(){
    return m_is_running;
}

void EngineWindow::m_job_update_delta(){
    m_this_total_time = SDL_GetTicks();
    m_delta = m_this_total_time - m_last_total_time; 
    m_last_total_time = m_this_total_time;
}

Uint64 EngineWindow::get_total_time(){
    return SDL_GetTicks();
}
Uint64 EngineWindow::get_delta(){
    return m_delta;
}


vec2 EngineWindow::get_window_size(){
    SDL_GetWindowSize(this->m_sdl_window, &m_width, &m_height);
    return vec2(m_width, m_height);
}

vec2 EngineWindow::get_window_position(){
    int x, y;
    SDL_GetWindowPosition(m_sdl_window, &x, &y);
    return {x,y};
}

void EngineWindow::set_window_position(vec2 p_pos){
    SDL_SetWindowPosition(m_sdl_window, p_pos.x, std::max(0.0f, p_pos.y));
    m_store_buffer();
}
void EngineWindow::set_window_AABB(vec2 p_left_top, vec2 p_right_down){
    vec2 clamp_left_top = glm::max(p_left_top, vec2(0.0f, 0.0f));
    vec2 size = p_right_down - clamp_left_top;
    set_window_position(clamp_left_top);
    SDL_SetWindowSize(m_sdl_window, size.x, size.y);
    m_store_buffer();
}


void EngineWindow::window_follow_mouse(){
    int pos_x, pos_y;
    SDL_GetWindowPosition(m_sdl_window, &pos_x, &pos_y);
    vec2 old_pos = {pos_x, pos_y};
    vec2 new_pos = m_screen_mouse_position - m_window_dragging_offset;

    if(new_pos != old_pos && is_maximized()){
        int width, height;
        SDL_GetWindowSize(m_sdl_window, &width, &height);

        vec2 window_mouse_pos = EngineInputHub::Ref()->get_mouse_position();
        proportion_x = window_mouse_pos.x / (double)width;

        this->restore();
    }

    if(!is_maximized() && !is_minimized()){
        if(m_screen_mouse_position.y == 0 && EngineInputHub::Ref()->is_mouse_left_button_just_released()){
            this->stop_dragging();
            this->maximize();
            return;
        }
    }

    set_window_position(new_pos);
}
void EngineWindow::start_dragging(){
    m_events.emplace(Event::START_DRAG);
}
void EngineWindow::stop_dragging(){
    m_events.emplace(Event::END_DRAG);
}
unsigned int EngineWindow::get_window_id(){
    return SDL_GetWindowID(m_sdl_window);
}

bool EngineWindow::is_maximized(){
    Uint32 flags = SDL_GetWindowFlags(m_sdl_window);
    return (flags & SDL_WINDOW_MAXIMIZED);
}
bool EngineWindow::is_minimized(){
    Uint32 flags = SDL_GetWindowFlags(m_sdl_window);
    return (flags & SDL_WINDOW_MINIMIZED);
}
void EngineWindow::maximize(){
    if(is_maximized()){
        return;
    }

    m_events.emplace(Event::MAXIMIZE);
}
void EngineWindow::minimize(){
    if(is_minimized()){
        return;
    }

    m_events.emplace(Event::MINIMIZE);
}
void EngineWindow::restore(){
    m_events.emplace(Event::RESTORE);
}

void EngineWindow::after_restore(){    
    int width, height;
    SDL_GetWindowSize(m_sdl_window, &width, &height);
    m_window_dragging_offset = {width * proportion_x, proportion_y * EditorLayout::Ref()->menu_bar_size};

    m_apply_buffer();
}


void EngineWindow::focus(){
    SDL_RaiseWindow(m_sdl_window);
    SDL_SetWindowMouseGrab(m_sdl_window, false);
    SDL_SetWindowKeyboardGrab(m_sdl_window, false);
    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(true);
    io.AddMousePosEvent(io.MousePos.x, io.MousePos.y);
    io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;
    io.AddFocusEvent(true);
}
void EngineWindow::m_store_buffer(){
    int x, y, w, h;
    SDL_GetWindowPosition(m_sdl_window, &x, &y);
    SDL_GetWindowSize(m_sdl_window, &w, &h);

    m_pos_buffer = {x,y};
    m_size_buffer = {w,h};
}
void EngineWindow::m_apply_buffer(){
    SDL_SetWindowSize(m_sdl_window, m_size_buffer.x, m_size_buffer.y);
    SDL_SetWindowPosition(m_sdl_window, m_pos_buffer.x, m_pos_buffer.y);
}


void EngineWindow::move_left_top(vec2 p_pos){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_left_top(p_pos, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_left_down(vec2 p_pos){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_left_down(p_pos, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_right_top(vec2 p_pos){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_right_top(p_pos, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_right_down(vec2 p_pos){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_right_down(p_pos, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_left(double p_x){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_left(p_x, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_right(double p_x){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_right(p_x, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_top(double p_y){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_top(p_y, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}
void EngineWindow::move_down(double p_y){
    vec2 old_size = get_window_size();
    vec2 old_pos = get_window_position();
    Rect2 rect = Rect2(old_pos + (old_size / 2.0f),old_size);

    rect.move_down(p_y, m_min_window_size);

    vec2 new_left_top = rect.get_left_top();
    vec2 new_right_down = rect.get_right_down();

    set_window_AABB(new_left_top, new_right_down);
}

void EngineWindow::m_job_resizer_handle(){
    if(!EventServer::Ref()->has<EventDragResizer>()){
        return;
    }

    float x,y;
    SDL_GetGlobalMouseState(&x, &y);
    vec2 global_mouse_pos = {x,y};

    EventDragResizer event = EventServer::Ref()->poll_first<EventDragResizer>();
    switch (event.dir) {
        case EventDirection::UP:
            this->move_top(global_mouse_pos.y);
            break;
        case EventDirection::DOWN:
            this->move_down(global_mouse_pos.y);
            break;
        case EventDirection::LEFT:
            this->move_left(global_mouse_pos.x);
            break;
        case EventDirection::RIGHT:
            this->move_right(global_mouse_pos.x);
            break;
        case EventDirection::UP_LEFT:
            this->move_left_top(global_mouse_pos);
            break;
        case EventDirection::DOWN_LEFT:
            this->move_left_down(global_mouse_pos);
            break;
        case EventDirection::UP_RIGHT:
            this->move_right_top(global_mouse_pos);
            break;
        case EventDirection::DOWN_RIGHT:
            this->move_right_down(global_mouse_pos);
            break;
          break;
    }
    
}

void EngineWindow::refresh() {
    if (!m_sdl_window || !m_sdl_gl_context) {
        ERROR_MSG("EngineWindow::refresh failed: invalid window or GL context.");
        return;
    }

    vec2 size = get_window_size();
    glViewport(0, 0, (int)size.x, (int)size.y);

    glClearColor(m_clear_color.x * m_clear_color.w,
                 m_clear_color.y * m_clear_color.w,
                 m_clear_color.z * m_clear_color.w,
                 m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(m_sdl_window);
}

unsigned long long EngineWindow::get_frame(){
    return m_frame;
}


ExplorerWindow* EngineWindow::create_explorer_window(){
    ExplorerWindow* ret = ObjectServer::Ref()->queue_create<ExplorerWindow>();
    ret->set_sdl_window(m_sdl_window);
    return ret;
}