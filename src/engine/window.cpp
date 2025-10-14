#include "engine/window.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_video.h"
#include "engine/event_hub.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_int3.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
            ENGINE_INIT_BIND(m_init_sdl_show_window)
        };

        for(auto& step_func : all_init_steps){
            step_func();
        }
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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
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

    m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, NULL);
    if (!m_sdl_renderer) {
        ERROR_MSG("SDL_CreateRenderer Error: " << SDL_GetError());
        SDL_DestroyWindow(m_sdl_window);
        SDL_Quit();
        exit(-1);
    }
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

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Engine + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_Engine);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
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
    ImGui_ImplOpenGL3_Init(glsl_version);
}
void EngineWindow::m_init_sdl_show_window(){
    const int step = ENGINE_INIT_STEP;
    m_init_check_step(step);

    SDL_SetWindowPosition(m_sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(m_sdl_window);

    
    m_is_init_done = true;
}
bool EngineWindow::is_minimized(){
    return SDL_GetWindowFlags(m_sdl_window) & SDL_WINDOW_MINIMIZED;
}

void EngineWindow::destory_all(){
    SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();
}


void EngineWindow::m_job_gl_clear(){
    glViewport(0, 0, (int)this->get_window_size().x, (int)this->get_window_size().y);
    glClearColor(m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);



    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    glm::vec2 cameraPos(0.0f, 0.0f);
    float cameraZoom = 1.0f;
    float cameraRotation = 0.0f;
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
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


void EngineWindow::m_job_set_delay_if_minimized(){
    if(this->is_minimized()){
        this->delay(10);
    }
}
void EngineWindow::m_job_close_event_handle(){
    if(EngineEventHub::Ref()->is_close_requested()){
        if(EngineEventHub::Ref()->get_close_window_id() == SDL_GetWindowID(m_sdl_window)){
            this->close();
        }
    }
}

void EngineWindow::close(){
    m_is_running = false;
}

void EngineWindow::begin(){
    m_job_close_event_handle();
    m_job_update_delta();
    m_job_set_delay_if_minimized();
    m_job_gl_clear();
    m_job_imgui_new_frame();
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
