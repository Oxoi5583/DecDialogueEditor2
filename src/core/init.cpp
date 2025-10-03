#include "core/init.h"

void CoreInitializer::init_all(){
    init_step0_sdl_core();
    init_step1_sdl_window();
    init_step2_sdl_renderer();
    init_step3_sdl_gl_context();
}
void CoreInitializer::init_step0_sdl_core(){
    int step = 0;

    if(m_init_step != step){
        ERROR_MSG("SDL_Init Error: " << "Incorrect Step.");
        exit(-1);
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        ERROR_MSG("SDL_Init Error: " << SDL_GetError());
        exit(-1);
    }
    m_init_step++;
}
void CoreInitializer::init_step1_sdl_window(){
    int step = 1;

    if(m_init_step != step){
        ERROR_MSG("SDL_Init Error: " << "Incorrect Step.");
        exit(-1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    window = SDL_CreateWindow(m_title,
                                800, 600,  
                                window_flags);
    if (!window) {
        ERROR_MSG("SDL_CreateWindow Error: " << SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
    m_init_step++;
}
void CoreInitializer::init_step2_sdl_renderer(){
    int step = 2;

    if(m_init_step != step){
        ERROR_MSG("SDL_Init Error: " << "Incorrect Step.");
        exit(-1);
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        ERROR_MSG("SDL_CreateRenderer Error: " << SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(-1);
    }
    m_init_step++;
}
void CoreInitializer::init_step3_sdl_gl_context(){
    int step = 3;

    if(m_init_step != step){
        ERROR_MSG("SDL_Init Error: " << "Incorrect Step.");
        exit(-1);
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        ERROR_MSG("SDL_GL_CreateContext Error: " << SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        exit(-1);
    }
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); 
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);
    m_init_step++;
    m_is_init_done = true;
}
SDL_Renderer* CoreInitializer::get_renderer(){
    if(!m_is_init_done){
        ERROR_MSG("SDL_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return renderer;
}
SDL_Window* CoreInitializer::get_window(){
    if(!m_is_init_done){
        ERROR_MSG("SDL_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return window;
}
SDL_GLContext CoreInitializer::get_glcontext(){
    if(!m_is_init_done){
        ERROR_MSG("SDL_Init Error: " << "Init Not Finished.");
        exit(-1);
    }
    return gl_context;
}


void CoreInitializer::destory_all(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}