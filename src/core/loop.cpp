#include "core/loop.h"
#include "SDL3/SDL_timer.h"
#include "core/init.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/imgui.h"

void CoreLoop::start(){
    m_is_running = true;
}
void CoreLoop::stop(){
    m_is_running = false;
}

bool CoreLoop::poll(){
    m_job_event_handling();
    m_job_set_sdl_delay();
    m_job_update_delta();
    return m_is_running;
}


void CoreLoop::m_job_event_handling(){
    SDL_Window* window = CoreInitializer::Ref()->get_sdl_window();

    while (SDL_PollEvent(&m_event)) {
        ImGui_ImplSDL3_ProcessEvent(&m_event);
        if (m_event.type == SDL_EVENT_QUIT){
            this->stop();
        }
        if (m_event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && m_event.window.windowID == SDL_GetWindowID(window)){
            this->stop();
        }
        if (m_event.type == SDL_EVENT_QUIT) {
            this->stop();
        }
    }
}
void CoreLoop::m_job_set_sdl_delay(){
    SDL_Window* window = CoreInitializer::Ref()->get_sdl_window();

    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED){
        SDL_Delay(10);
    }

}
void CoreLoop::m_job_update_delta(){
    m_this_total_time = SDL_GetTicks();
    m_delta = m_this_total_time - m_last_total_time; 
    m_last_total_time = m_this_total_time;
}

void CoreLoop::new_frame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}
void CoreLoop::render(){
    SDL_Window* window = CoreInitializer::Ref()->get_sdl_window();
    ImGuiIO& io = CoreInitializer::Ref()->get_imgui_io();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}


void CoreLoop::set_clear_color(ImVec4 p_color){
    m_clear_color = p_color;
}
ImVec4 CoreLoop::get_clear_color(){
    return m_clear_color;
}
Uint64 CoreLoop::get_total_time(){
    return SDL_GetTicks();
}
Uint64 CoreLoop::get_delta(){
    return m_delta;
}

void CoreLoop::pre_process(){

}
void CoreLoop::process(){

}
void CoreLoop::post_process(){

}
