#include "core/init.h"
#include "DecToolsBox/debug/messenger.h"
#include <vector>

#define CORE_INIT_STEP __COUNTER__
#define CORE_INIT_BIND(FUNC) std::bind(&CoreInitializer::FUNC, this)

void CoreInitializer::init_all(){
    if(!m_is_init_done){
        const std::vector<std::function<void()>> all_init_steps = {
            CORE_INIT_BIND(m_init_sdl_core),
            CORE_INIT_BIND(m_init_sdl_set_gl_attribute),
            CORE_INIT_BIND(m_init_create_sdl_window),
            CORE_INIT_BIND(m_init_create_sdl_renderer),
            CORE_INIT_BIND(m_init_create_sdl_gl_context),
            CORE_INIT_BIND(m_init_imgui_core),
            CORE_INIT_BIND(m_init_sdl_show_window)
        };

        for(auto& step_func : all_init_steps){
            step_func();
        }
    }
}

void CoreInitializer::m_init_check_step(const int& p_step){
    if(m_init_step != p_step){
        ERROR_MSG("SDL_Init Error: " << "Incorrect Step. Current step is " << m_init_step << " and it should be " << p_step << ".");
        exit(-1);
    }
    m_init_step++;
}

void CoreInitializer::m_init_sdl_core(){
    const int step = CORE_INIT_STEP;
    m_init_check_step(step);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        ERROR_MSG("SDL_Init Error: " << SDL_GetError());
        exit(-1);
    }
}
void CoreInitializer::m_init_sdl_set_gl_attribute(){
    const int step = CORE_INIT_STEP;
    m_init_check_step(step);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}
void CoreInitializer::m_init_create_sdl_window(){
    const int step = CORE_INIT_STEP;
    m_init_check_step(step);

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    m_sdl_window = SDL_CreateWindow(m_main_window_title,800, 600, m_sdl_window_flags);
    if (!m_sdl_window) {
        ERROR_MSG("SDL_CreateWindow Error: " << SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
}
void CoreInitializer::m_init_create_sdl_renderer(){
    const int step = CORE_INIT_STEP;
    m_init_check_step(step);

    m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, NULL);
    if (!m_sdl_renderer) {
        ERROR_MSG("SDL_CreateRenderer Error: " << SDL_GetError());
        SDL_DestroyWindow(m_sdl_window);
        SDL_Quit();
        exit(-1);
    }
}
void CoreInitializer::m_init_create_sdl_gl_context(){
    const int step = CORE_INIT_STEP;
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
void CoreInitializer::m_init_imgui_core(){
    const int step = CORE_INIT_STEP;
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
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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
void CoreInitializer::m_init_sdl_show_window(){
    const int step = CORE_INIT_STEP;
    m_init_check_step(step);

    SDL_SetWindowPosition(m_sdl_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(m_sdl_window);

    
    m_is_init_done = true;
}
SDL_Renderer* CoreInitializer::get_sdl_renderer(){
    if(!m_is_init_done){
        ERROR_MSG("SDL_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return m_sdl_renderer;
}
SDL_Window* CoreInitializer::get_sdl_window(){
    if(!m_is_init_done){
        ERROR_MSG("SDL_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return m_sdl_window;
}
SDL_GLContext CoreInitializer::get_sdl_glcontext(){
    if(!m_is_init_done){
        ERROR_MSG("SDL_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return m_sdl_gl_context;
}
ImGuiIO& CoreInitializer::get_imgui_io(){
    if(!m_is_init_done){
        ERROR_MSG("ImGUI_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return ImGui::GetIO();
}
ImGuiStyle& CoreInitializer::get_imgui_style(){
    if(!m_is_init_done){
        ERROR_MSG("ImGUI_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return ImGui::GetStyle();
}


void CoreInitializer::destory_all(){
    SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();
}