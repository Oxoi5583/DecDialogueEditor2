#include "engine/event_hub.h"
#include "DecToolsBox/debug/messenger.h"
#include "graph/camera.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"

void EngineEventHub::polling(){
    m_mouse_wheel = vec2();
    m_mouse_left_button_just_clicked = false;
    m_mouse_right_button_just_clicked = false;
    m_mouse_left_button_just_released = false;
    m_mouse_right_button_just_released = false;

    m_is_close_requested = false;

    while (SDL_PollEvent(&m_sdl_event)) {
        ImGui_ImplSDL3_ProcessEvent(&m_sdl_event);
        switch (m_sdl_event.type) {
            case (SDL_EVENT_QUIT):{
                m_is_close_requested = true;
                break;
            }
            case (SDL_EVENT_WINDOW_CLOSE_REQUESTED):{
                m_close_window_id = m_sdl_event.window.windowID;
                m_is_close_requested = true;
                break;
            }
            case (SDL_EVENT_MOUSE_MOTION):{
                m_mouse_position = {m_sdl_event.motion.x, m_sdl_event.motion.y};
                break;
            }
            case (SDL_EVENT_MOUSE_BUTTON_DOWN):{
                if (m_sdl_event.button.button == SDL_BUTTON_LEFT){
                    m_mouse_left_button_clicked = true;
                    m_mouse_left_button_just_clicked = true;
                }
                else if (m_sdl_event.button.button == SDL_BUTTON_RIGHT){
                    m_mouse_right_button_clicked = true;
                    m_mouse_right_button_just_clicked = true;
                }
                break;
            }
            case (SDL_EVENT_MOUSE_BUTTON_UP):{
                if (m_sdl_event.button.button == SDL_BUTTON_LEFT){
                    m_mouse_left_button_clicked = false;
                    m_mouse_left_button_just_released = true;
                }
                else if (m_sdl_event.button.button == SDL_BUTTON_RIGHT){
                    m_mouse_right_button_clicked = false;
                    m_mouse_right_button_just_released = true;
                }
                break;
            }
            case (SDL_EVENT_MOUSE_WHEEL):{
                m_mouse_wheel = {m_sdl_event.wheel.x, m_sdl_event.wheel.y};
                break;
            }
            default:{
                break;
            }
        }
    }
}

vec2 EngineEventHub::get_mouse_position(){
    return m_mouse_position;
}
vec2 EngineEventHub::get_mouse_world_position(){
    mat4 camera_origin_transform = mat4(1.0f);
    camera_origin_transform = glm::translate(camera_origin_transform, vec3(GraphCamera::Ref()->get_origin(),0.0f));
    return camera_origin_transform * vec4(m_mouse_position,0.0f,1.0f);
}
vec2 EngineEventHub::get_mouse_wheel(){
    return m_mouse_wheel;
}
bool EngineEventHub::is_mouse_left_button_just_clicked(){
    return m_mouse_left_button_just_clicked;
}
bool EngineEventHub::is_mouse_right_button_just_clicked(){
    return m_mouse_right_button_just_clicked;
}
bool EngineEventHub::is_mouse_left_button_just_released(){
    return m_mouse_left_button_just_released;
}
bool EngineEventHub::is_mouse_right_button_just_released(){
    return m_mouse_right_button_just_released;
}
bool EngineEventHub::is_mouse_left_button_clicked(){
    return m_mouse_left_button_clicked;
}
bool EngineEventHub::is_mouse_right_button_clicked(){
    return m_mouse_right_button_clicked;
}


bool EngineEventHub::is_close_requested(){
    return m_is_close_requested;
}
Uint32 EngineEventHub::get_close_window_id(){
    return m_close_window_id;
}


