#include "engine/input_hub.h"
#include "DecToolsBox/debug/messenger.h"
#include "editor/layout.h"
#include "engine/window.h"
#include "ext/debug/messenger_ext.h"
#include "engine/input_key.h"
#include "glm/common.hpp"
#include "graph/camera.h"

#include "SDL3/SDL_events.h"
#include "glm/ext/vector_float2.hpp"
#include "graph/viewport.h"
#include "server/event_server.h"
#include "server/events.h"
#include "struct/shape/rect2.h"

#include "imgui/backends/imgui_impl_sdl3.h"
#include <unordered_set>

void EngineInputHub::polling_sdl_event(){
    m_mouse_motion = vec2();
    m_mouse_wheel = vec2();
    m_mouse_left_button_just_clicked = false;
    m_mouse_right_button_just_clicked = false;
    m_mouse_left_button_just_released = false;
    m_mouse_right_button_just_released = false;

    m_is_close_requested = false;

    std::unordered_set<EngineKeycode>().swap(m_keyboard_just_down_buffer);
    std::unordered_set<EngineKeycode>().swap(m_keyboard_just_up_buffer);
    
    SDL_Event sdl_event;

    while (SDL_PollEvent(&sdl_event)) {
        ImGui_ImplSDL3_ProcessEvent(&sdl_event);
        switch (sdl_event.type) {
            case (SDL_EVENT_WINDOW_RESIZED):{
                EventWindowResized event;
                event.new_size = EngineWindow::Ref()->get_window_size();
                EventServer::Ref()->emit(event);
                break;
            }
            case (SDL_EVENT_WINDOW_MAXIMIZED):{
                EngineWindow::Ref()->stop_dragging();
                EngineWindow::Ref()->focus();

                EventWindowResized event;
                event.new_size = EngineWindow::Ref()->get_window_size();
                EventServer::Ref()->emit(event);
                break;
            }
            case (SDL_EVENT_WINDOW_MINIMIZED):{
                EngineWindow::Ref()->stop_dragging();
                EngineWindow::Ref()->focus();

                EventWindowResized event;
                event.new_size = EngineWindow::Ref()->get_window_size();
                EventServer::Ref()->emit(event);
                break;
            }
            case (SDL_EVENT_WINDOW_RESTORED):{
                //EngineWindow::Ref()->restore();
                EngineWindow::Ref()->after_restore();
                EngineWindow::Ref()->focus();

                EventWindowResized event;
                event.new_size = EngineWindow::Ref()->get_window_size();
                EventServer::Ref()->emit(event);
                break;
            }
            case (SDL_EVENT_QUIT):{
                m_is_close_requested = true;
                if(this->is_close_requested()){
                    if(this->get_close_window_id() == EngineWindow::Ref()->get_window_id()){
                        EngineWindow::Ref()->close();
                    }
                }
                break;
            }
            case (SDL_EVENT_WINDOW_CLOSE_REQUESTED):{
                m_close_window_id = sdl_event.window.windowID;
                m_is_close_requested = true;
                if(this->is_close_requested()){
                    if(this->get_close_window_id() == EngineWindow::Ref()->get_window_id()){
                        EngineWindow::Ref()->close();
                    }
                }
                break;
            }
            case (SDL_EVENT_MOUSE_MOTION):{
                m_mouse_last_position = m_mouse_position;
                m_mouse_position = {sdl_event.motion.x, sdl_event.motion.y};
                m_mouse_motion = glm::floor(m_mouse_position) - glm::floor(m_mouse_last_position);
                break;
            }
            case (SDL_EVENT_MOUSE_BUTTON_DOWN):{
                if (sdl_event.button.button == SDL_BUTTON_LEFT){
                    m_mouse_left_button_clicked = true;
                    m_mouse_left_button_just_clicked = true;
                }
                else if (sdl_event.button.button == SDL_BUTTON_RIGHT){
                    m_mouse_right_button_clicked = true;
                    m_mouse_right_button_just_clicked = true;
                }
                break;
            }
            case (SDL_EVENT_MOUSE_BUTTON_UP):{
                if (sdl_event.button.button == SDL_BUTTON_LEFT){
                    m_mouse_left_button_clicked = false;
                    m_mouse_left_button_just_released = true;
                }
                else if (sdl_event.button.button == SDL_BUTTON_RIGHT){
                    m_mouse_right_button_clicked = false;
                    m_mouse_right_button_just_released = true;
                }
                break;
            }
            case (SDL_EVENT_MOUSE_WHEEL):{
                m_mouse_wheel = {sdl_event.wheel.x, sdl_event.wheel.y};
                break;
            }
            case (SDL_EVENT_KEY_DOWN):{
                m_redirect_keyboard_down_buffer(sdl_event.key.key);
                break;
            }
            case (SDL_EVENT_KEY_UP):{
                m_redirect_keyboard_up_buffer(sdl_event.key.key);
                break;
            }
            case (SDL_EVENT_WINDOW_MOUSE_ENTER):{
                m_is_mouse_in_window = true;
                break;
            }
            case (SDL_EVENT_WINDOW_MOUSE_LEAVE):{
                m_is_mouse_in_window = false;
                break;
            }
            default:{
                break;
            }
        }
    }
}

void EngineInputHub::m_store_keyboard_down_buffer(SDL_Keycode p_key){
    if(!m_keyboard_down_buffer.contains(p_key)){
        m_keyboard_just_down_buffer.emplace(p_key);
        m_keyboard_down_buffer.emplace(p_key);
    }
}
void EngineInputHub::m_store_keyboard_up_buffer(SDL_Keycode p_key){
    if(m_keyboard_down_buffer.contains(p_key)){
        m_keyboard_down_buffer.erase(p_key);
    }
    m_keyboard_just_up_buffer.emplace(p_key);
}
void EngineInputHub::m_redirect_keyboard_down_buffer(SDL_Keycode p_key){
    switch (p_key) {
        case SDLK_UNKNOWN: m_store_keyboard_down_buffer(K_UNKNOWN); break;
        case SDLK_RETURN: m_store_keyboard_down_buffer(K_RETURN); break;
        case SDLK_ESCAPE: m_store_keyboard_down_buffer(K_ESCAPE); break;
        case SDLK_BACKSPACE: m_store_keyboard_down_buffer(K_BACKSPACE); break;
        case SDLK_TAB: m_store_keyboard_down_buffer(K_TAB); break;
        case SDLK_SPACE: m_store_keyboard_down_buffer(K_SPACE); break;
        case SDLK_EXCLAIM: m_store_keyboard_down_buffer(K_EXCLAIM); break;
        case SDLK_DBLAPOSTROPHE: m_store_keyboard_down_buffer(K_DBLAPOSTROPHE); break;
        case SDLK_HASH: m_store_keyboard_down_buffer(K_HASH); break;
        case SDLK_DOLLAR: m_store_keyboard_down_buffer(K_DOLLAR); break;
        case SDLK_PERCENT: m_store_keyboard_down_buffer(K_PERCENT); break;
        case SDLK_AMPERSAND: m_store_keyboard_down_buffer(K_AMPERSAND); break;
        case SDLK_APOSTROPHE: m_store_keyboard_down_buffer(K_APOSTROPHE); break;
        case SDLK_LEFTPAREN: m_store_keyboard_down_buffer(K_LEFTPAREN); break;
        case SDLK_RIGHTPAREN: m_store_keyboard_down_buffer(K_RIGHTPAREN); break;
        case SDLK_ASTERISK: m_store_keyboard_down_buffer(K_ASTERISK); break;
        case SDLK_PLUS: m_store_keyboard_down_buffer(K_PLUS); break;
        case SDLK_COMMA: m_store_keyboard_down_buffer(K_COMMA); break;
        case SDLK_MINUS: m_store_keyboard_down_buffer(K_MINUS); break;
        case SDLK_PERIOD: m_store_keyboard_down_buffer(K_PERIOD); break;
        case SDLK_SLASH: m_store_keyboard_down_buffer(K_SLASH); break;
        case SDLK_0: m_store_keyboard_down_buffer(K_0); break;
        case SDLK_1: m_store_keyboard_down_buffer(K_1); break;
        case SDLK_2: m_store_keyboard_down_buffer(K_2); break;
        case SDLK_3: m_store_keyboard_down_buffer(K_3); break;
        case SDLK_4: m_store_keyboard_down_buffer(K_4); break;
        case SDLK_5: m_store_keyboard_down_buffer(K_5); break;
        case SDLK_6: m_store_keyboard_down_buffer(K_6); break;
        case SDLK_7: m_store_keyboard_down_buffer(K_7); break;
        case SDLK_8: m_store_keyboard_down_buffer(K_8); break;
        case SDLK_9: m_store_keyboard_down_buffer(K_9); break;
        case SDLK_COLON: m_store_keyboard_down_buffer(K_COLON); break;
        case SDLK_SEMICOLON: m_store_keyboard_down_buffer(K_SEMICOLON); break;
        case SDLK_LESS: m_store_keyboard_down_buffer(K_LESS); break;
        case SDLK_EQUALS: m_store_keyboard_down_buffer(K_EQUALS); break;
        case SDLK_GREATER: m_store_keyboard_down_buffer(K_GREATER); break;
        case SDLK_QUESTION: m_store_keyboard_down_buffer(K_QUESTION); break;
        case SDLK_AT: m_store_keyboard_down_buffer(K_AT); break;
        case SDLK_LEFTBRACKET: m_store_keyboard_down_buffer(K_LEFTBRACKET); break;
        case SDLK_BACKSLASH: m_store_keyboard_down_buffer(K_BACKSLASH); break;
        case SDLK_RIGHTBRACKET: m_store_keyboard_down_buffer(K_RIGHTBRACKET); break;
        case SDLK_CARET: m_store_keyboard_down_buffer(K_CARET); break;
        case SDLK_UNDERSCORE: m_store_keyboard_down_buffer(K_UNDERSCORE); break;
        case SDLK_GRAVE: m_store_keyboard_down_buffer(K_GRAVE); break;
        case SDLK_A: m_store_keyboard_down_buffer(K_A); break;
        case SDLK_B: m_store_keyboard_down_buffer(K_B); break;
        case SDLK_C: m_store_keyboard_down_buffer(K_C); break;
        case SDLK_D: m_store_keyboard_down_buffer(K_D); break;
        case SDLK_E: m_store_keyboard_down_buffer(K_E); break;
        case SDLK_F: m_store_keyboard_down_buffer(K_F); break;
        case SDLK_G: m_store_keyboard_down_buffer(K_G); break;
        case SDLK_H: m_store_keyboard_down_buffer(K_H); break;
        case SDLK_I: m_store_keyboard_down_buffer(K_I); break;
        case SDLK_J: m_store_keyboard_down_buffer(K_J); break;
        case SDLK_K: m_store_keyboard_down_buffer(K_K); break;
        case SDLK_L: m_store_keyboard_down_buffer(K_L); break;
        case SDLK_M: m_store_keyboard_down_buffer(K_M); break;
        case SDLK_N: m_store_keyboard_down_buffer(K_N); break;
        case SDLK_O: m_store_keyboard_down_buffer(K_O); break;
        case SDLK_P: m_store_keyboard_down_buffer(K_P); break;
        case SDLK_Q: m_store_keyboard_down_buffer(K_Q); break;
        case SDLK_R: m_store_keyboard_down_buffer(K_R); break;
        case SDLK_S: m_store_keyboard_down_buffer(K_S); break;
        case SDLK_T: m_store_keyboard_down_buffer(K_T); break;
        case SDLK_U: m_store_keyboard_down_buffer(K_U); break;
        case SDLK_V: m_store_keyboard_down_buffer(K_V); break;
        case SDLK_W: m_store_keyboard_down_buffer(K_W); break;
        case SDLK_X: m_store_keyboard_down_buffer(K_X); break;
        case SDLK_Y: m_store_keyboard_down_buffer(K_Y); break;
        case SDLK_Z: m_store_keyboard_down_buffer(K_Z); break;
        case SDLK_LEFTBRACE: m_store_keyboard_down_buffer(K_LEFTBRACE); break;
        case SDLK_PIPE: m_store_keyboard_down_buffer(K_PIPE); break;
        case SDLK_RIGHTBRACE: m_store_keyboard_down_buffer(K_RIGHTBRACE); break;
        case SDLK_TILDE: m_store_keyboard_down_buffer(K_TILDE); break;
        case SDLK_DELETE: m_store_keyboard_down_buffer(K_DELETE); break;
        case SDLK_PLUSMINUS: m_store_keyboard_down_buffer(K_PLUSMINUS); break;
        case SDLK_CAPSLOCK: m_store_keyboard_down_buffer(K_CAPSLOCK); break;
        case SDLK_F1: m_store_keyboard_down_buffer(K_F1); break;
        case SDLK_F2: m_store_keyboard_down_buffer(K_F2); break;
        case SDLK_F3: m_store_keyboard_down_buffer(K_F3); break;
        case SDLK_F4: m_store_keyboard_down_buffer(K_F4); break;
        case SDLK_F5: m_store_keyboard_down_buffer(K_F5); break;
        case SDLK_F6: m_store_keyboard_down_buffer(K_F6); break;
        case SDLK_F7: m_store_keyboard_down_buffer(K_F7); break;
        case SDLK_F8: m_store_keyboard_down_buffer(K_F8); break;
        case SDLK_F9: m_store_keyboard_down_buffer(K_F9); break;
        case SDLK_F10: m_store_keyboard_down_buffer(K_F10); break;
        case SDLK_F11: m_store_keyboard_down_buffer(K_F11); break;
        case SDLK_F12: m_store_keyboard_down_buffer(K_F12); break;
        case SDLK_PRINTSCREEN: m_store_keyboard_down_buffer(K_PRINTSCREEN); break;
        case SDLK_SCROLLLOCK: m_store_keyboard_down_buffer(K_SCROLLLOCK); break;
        case SDLK_PAUSE: m_store_keyboard_down_buffer(K_PAUSE); break;
        case SDLK_INSERT: m_store_keyboard_down_buffer(K_INSERT); break;
        case SDLK_HOME: m_store_keyboard_down_buffer(K_HOME); break;
        case SDLK_PAGEUP: m_store_keyboard_down_buffer(K_PAGEUP); break;
        case SDLK_END: m_store_keyboard_down_buffer(K_END); break;
        case SDLK_PAGEDOWN: m_store_keyboard_down_buffer(K_PAGEDOWN); break;
        case SDLK_RIGHT: m_store_keyboard_down_buffer(K_RIGHT); break;
        case SDLK_LEFT: m_store_keyboard_down_buffer(K_LEFT); break;
        case SDLK_DOWN: m_store_keyboard_down_buffer(K_DOWN); break;
        case SDLK_UP: m_store_keyboard_down_buffer(K_UP); break;
        case SDLK_NUMLOCKCLEAR: m_store_keyboard_down_buffer(K_NUMLOCKCLEAR); break;
        case SDLK_KP_DIVIDE: m_store_keyboard_down_buffer(K_KP_DIVIDE); break;
        case SDLK_KP_MULTIPLY: m_store_keyboard_down_buffer(K_KP_MULTIPLY); break;
        case SDLK_KP_MINUS: m_store_keyboard_down_buffer(K_KP_MINUS); break;
        case SDLK_KP_PLUS: m_store_keyboard_down_buffer(K_KP_PLUS); break;
        case SDLK_KP_ENTER: m_store_keyboard_down_buffer(K_KP_ENTER); break;
        case SDLK_KP_1: m_store_keyboard_down_buffer(K_KP_1); break;
        case SDLK_KP_2: m_store_keyboard_down_buffer(K_KP_2); break;
        case SDLK_KP_3: m_store_keyboard_down_buffer(K_KP_3); break;
        case SDLK_KP_4: m_store_keyboard_down_buffer(K_KP_4); break;
        case SDLK_KP_5: m_store_keyboard_down_buffer(K_KP_5); break;
        case SDLK_KP_6: m_store_keyboard_down_buffer(K_KP_6); break;
        case SDLK_KP_7: m_store_keyboard_down_buffer(K_KP_7); break;
        case SDLK_KP_8: m_store_keyboard_down_buffer(K_KP_8); break;
        case SDLK_KP_9: m_store_keyboard_down_buffer(K_KP_9); break;
        case SDLK_KP_0: m_store_keyboard_down_buffer(K_KP_0); break;
        case SDLK_KP_PERIOD: m_store_keyboard_down_buffer(K_KP_PERIOD); break;
        case SDLK_APPLICATION: m_store_keyboard_down_buffer(K_APPLICATION); break;
        case SDLK_POWER: m_store_keyboard_down_buffer(K_POWER); break;
        case SDLK_KP_EQUALS: m_store_keyboard_down_buffer(K_KP_EQUALS); break;
        case SDLK_F13: m_store_keyboard_down_buffer(K_F13); break;
        case SDLK_F14: m_store_keyboard_down_buffer(K_F14); break;
        case SDLK_F15: m_store_keyboard_down_buffer(K_F15); break;
        case SDLK_F16: m_store_keyboard_down_buffer(K_F16); break;
        case SDLK_F17: m_store_keyboard_down_buffer(K_F17); break;
        case SDLK_F18: m_store_keyboard_down_buffer(K_F18); break;
        case SDLK_F19: m_store_keyboard_down_buffer(K_F19); break;
        case SDLK_F20: m_store_keyboard_down_buffer(K_F20); break;
        case SDLK_F21: m_store_keyboard_down_buffer(K_F21); break;
        case SDLK_F22: m_store_keyboard_down_buffer(K_F22); break;
        case SDLK_F23: m_store_keyboard_down_buffer(K_F23); break;
        case SDLK_F24: m_store_keyboard_down_buffer(K_F24); break;
        case SDLK_EXECUTE: m_store_keyboard_down_buffer(K_EXECUTE); break;
        case SDLK_HELP: m_store_keyboard_down_buffer(K_HELP); break;
        case SDLK_MENU: m_store_keyboard_down_buffer(K_MENU); break;
        case SDLK_SELECT: m_store_keyboard_down_buffer(K_SELECT); break;
        case SDLK_STOP: m_store_keyboard_down_buffer(K_STOP); break;
        case SDLK_AGAIN: m_store_keyboard_down_buffer(K_AGAIN); break;
        case SDLK_UNDO: m_store_keyboard_down_buffer(K_UNDO); break;
        case SDLK_CUT: m_store_keyboard_down_buffer(K_CUT); break;
        case SDLK_COPY: m_store_keyboard_down_buffer(K_COPY); break;
        case SDLK_PASTE: m_store_keyboard_down_buffer(K_PASTE); break;
        case SDLK_FIND: m_store_keyboard_down_buffer(K_FIND); break;
        case SDLK_MUTE: m_store_keyboard_down_buffer(K_MUTE); break;
        case SDLK_VOLUMEUP: m_store_keyboard_down_buffer(K_VOLUMEUP); break;
        case SDLK_VOLUMEDOWN: m_store_keyboard_down_buffer(K_VOLUMEDOWN); break;
        case SDLK_KP_COMMA: m_store_keyboard_down_buffer(K_KP_COMMA); break;
        case SDLK_KP_EQUALSAS400: m_store_keyboard_down_buffer(K_KP_EQUALSAS400); break;
        case SDLK_ALTERASE: m_store_keyboard_down_buffer(K_ALTERASE); break;
        case SDLK_SYSREQ: m_store_keyboard_down_buffer(K_SYSREQ); break;
        case SDLK_CANCEL: m_store_keyboard_down_buffer(K_CANCEL); break;
        case SDLK_CLEAR: m_store_keyboard_down_buffer(K_CLEAR); break;
        case SDLK_PRIOR: m_store_keyboard_down_buffer(K_PRIOR); break;
        case SDLK_RETURN2: m_store_keyboard_down_buffer(K_RETURN2); break;
        case SDLK_SEPARATOR: m_store_keyboard_down_buffer(K_SEPARATOR); break;
        case SDLK_OUT: m_store_keyboard_down_buffer(K_OUT); break;
        case SDLK_OPER: m_store_keyboard_down_buffer(K_OPER); break;
        case SDLK_CLEARAGAIN: m_store_keyboard_down_buffer(K_CLEARAGAIN); break;
        case SDLK_CRSEL: m_store_keyboard_down_buffer(K_CRSEL); break;
        case SDLK_EXSEL: m_store_keyboard_down_buffer(K_EXSEL); break;
        case SDLK_KP_00: m_store_keyboard_down_buffer(K_KP_00); break;
        case SDLK_KP_000: m_store_keyboard_down_buffer(K_KP_000); break;
        case SDLK_THOUSANDSSEPARATOR: m_store_keyboard_down_buffer(K_THOUSANDSSEPARATOR); break;
        case SDLK_DECIMALSEPARATOR: m_store_keyboard_down_buffer(K_DECIMALSEPARATOR); break;
        case SDLK_CURRENCYUNIT: m_store_keyboard_down_buffer(K_CURRENCYUNIT); break;
        case SDLK_CURRENCYSUBUNIT: m_store_keyboard_down_buffer(K_CURRENCYSUBUNIT); break;
        case SDLK_KP_LEFTPAREN: m_store_keyboard_down_buffer(K_KP_LEFTPAREN); break;
        case SDLK_KP_RIGHTPAREN: m_store_keyboard_down_buffer(K_KP_RIGHTPAREN); break;
        case SDLK_KP_LEFTBRACE: m_store_keyboard_down_buffer(K_KP_LEFTBRACE); break;
        case SDLK_KP_RIGHTBRACE: m_store_keyboard_down_buffer(K_KP_RIGHTBRACE); break;
        case SDLK_KP_TAB: m_store_keyboard_down_buffer(K_KP_TAB); break;
        case SDLK_KP_BACKSPACE: m_store_keyboard_down_buffer(K_KP_BACKSPACE); break;
        case SDLK_KP_A: m_store_keyboard_down_buffer(K_KP_A); break;
        case SDLK_KP_B: m_store_keyboard_down_buffer(K_KP_B); break;
        case SDLK_KP_C: m_store_keyboard_down_buffer(K_KP_C); break;
        case SDLK_KP_D: m_store_keyboard_down_buffer(K_KP_D); break;
        case SDLK_KP_E: m_store_keyboard_down_buffer(K_KP_E); break;
        case SDLK_KP_F: m_store_keyboard_down_buffer(K_KP_F); break;
        case SDLK_KP_XOR: m_store_keyboard_down_buffer(K_KP_XOR); break;
        case SDLK_KP_POWER: m_store_keyboard_down_buffer(K_KP_POWER); break;
        case SDLK_KP_PERCENT: m_store_keyboard_down_buffer(K_KP_PERCENT); break;
        case SDLK_KP_LESS: m_store_keyboard_down_buffer(K_KP_LESS); break;
        case SDLK_KP_GREATER: m_store_keyboard_down_buffer(K_KP_GREATER); break;
        case SDLK_KP_AMPERSAND: m_store_keyboard_down_buffer(K_KP_AMPERSAND); break;
        case SDLK_KP_DBLAMPERSAND: m_store_keyboard_down_buffer(K_KP_DBLAMPERSAND); break;
        case SDLK_KP_VERTICALBAR: m_store_keyboard_down_buffer(K_KP_VERTICALBAR); break;
        case SDLK_KP_DBLVERTICALBAR: m_store_keyboard_down_buffer(K_KP_DBLVERTICALBAR); break;
        case SDLK_KP_COLON: m_store_keyboard_down_buffer(K_KP_COLON); break;
        case SDLK_KP_HASH: m_store_keyboard_down_buffer(K_KP_HASH); break;
        case SDLK_KP_SPACE: m_store_keyboard_down_buffer(K_KP_SPACE); break;
        case SDLK_KP_AT: m_store_keyboard_down_buffer(K_KP_AT); break;
        case SDLK_KP_EXCLAM: m_store_keyboard_down_buffer(K_KP_EXCLAM); break;
        case SDLK_KP_MEMSTORE: m_store_keyboard_down_buffer(K_KP_MEMSTORE); break;
        case SDLK_KP_MEMRECALL: m_store_keyboard_down_buffer(K_KP_MEMRECALL); break;
        case SDLK_KP_MEMCLEAR: m_store_keyboard_down_buffer(K_KP_MEMCLEAR); break;
        case SDLK_KP_MEMADD: m_store_keyboard_down_buffer(K_KP_MEMADD); break;
        case SDLK_KP_MEMSUBTRACT: m_store_keyboard_down_buffer(K_KP_MEMSUBTRACT); break;
        case SDLK_KP_MEMMULTIPLY: m_store_keyboard_down_buffer(K_KP_MEMMULTIPLY); break;
        case SDLK_KP_MEMDIVIDE: m_store_keyboard_down_buffer(K_KP_MEMDIVIDE); break;
        case SDLK_KP_PLUSMINUS: m_store_keyboard_down_buffer(K_KP_PLUSMINUS); break;
        case SDLK_KP_CLEAR: m_store_keyboard_down_buffer(K_KP_CLEAR); break;
        case SDLK_KP_CLEARENTRY: m_store_keyboard_down_buffer(K_KP_CLEARENTRY); break;
        case SDLK_KP_BINARY: m_store_keyboard_down_buffer(K_KP_BINARY); break;
        case SDLK_KP_OCTAL: m_store_keyboard_down_buffer(K_KP_OCTAL); break;
        case SDLK_KP_DECIMAL: m_store_keyboard_down_buffer(K_KP_DECIMAL); break;
        case SDLK_KP_HEXADECIMAL: m_store_keyboard_down_buffer(K_KP_HEXADECIMAL); break;
        case SDLK_LCTRL: m_store_keyboard_down_buffer(K_LCTRL); break;
        case SDLK_LSHIFT: m_store_keyboard_down_buffer(K_LSHIFT); break;
        case SDLK_LALT: m_store_keyboard_down_buffer(K_LALT); break;
        case SDLK_LGUI: m_store_keyboard_down_buffer(K_LGUI); break;
        case SDLK_RCTRL: m_store_keyboard_down_buffer(K_RCTRL); break;
        case SDLK_RSHIFT: m_store_keyboard_down_buffer(K_RSHIFT); break;
        case SDLK_RALT: m_store_keyboard_down_buffer(K_RALT); break;
        case SDLK_RGUI: m_store_keyboard_down_buffer(K_RGUI); break;
        case SDLK_MODE: m_store_keyboard_down_buffer(K_MODE); break;
        case SDLK_SLEEP: m_store_keyboard_down_buffer(K_SLEEP); break;
        case SDLK_WAKE: m_store_keyboard_down_buffer(K_WAKE); break;
        case SDLK_CHANNEL_INCREMENT: m_store_keyboard_down_buffer(K_CHANNEL_INCREMENT); break;
        case SDLK_CHANNEL_DECREMENT: m_store_keyboard_down_buffer(K_CHANNEL_DECREMENT); break;
        case SDLK_MEDIA_PLAY: m_store_keyboard_down_buffer(K_MEDIA_PLAY); break;
        case SDLK_MEDIA_PAUSE: m_store_keyboard_down_buffer(K_MEDIA_PAUSE); break;
        case SDLK_MEDIA_RECORD: m_store_keyboard_down_buffer(K_MEDIA_RECORD); break;
        case SDLK_MEDIA_FAST_FORWARD: m_store_keyboard_down_buffer(K_MEDIA_FAST_FORWARD); break;
        case SDLK_MEDIA_REWIND: m_store_keyboard_down_buffer(K_MEDIA_REWIND); break;
        case SDLK_MEDIA_NEXT_TRACK: m_store_keyboard_down_buffer(K_MEDIA_NEXT_TRACK); break;
        case SDLK_MEDIA_PREVIOUS_TRACK: m_store_keyboard_down_buffer(K_MEDIA_PREVIOUS_TRACK); break;
        case SDLK_MEDIA_STOP: m_store_keyboard_down_buffer(K_MEDIA_STOP); break;
        case SDLK_MEDIA_EJECT: m_store_keyboard_down_buffer(K_MEDIA_EJECT); break;
        case SDLK_MEDIA_PLAY_PAUSE: m_store_keyboard_down_buffer(K_MEDIA_PLAY_PAUSE); break;
        case SDLK_MEDIA_SELECT: m_store_keyboard_down_buffer(K_MEDIA_SELECT); break;
        case SDLK_AC_NEW: m_store_keyboard_down_buffer(K_AC_NEW); break;
        case SDLK_AC_OPEN: m_store_keyboard_down_buffer(K_AC_OPEN); break;
        case SDLK_AC_CLOSE: m_store_keyboard_down_buffer(K_AC_CLOSE); break;
        case SDLK_AC_EXIT: m_store_keyboard_down_buffer(K_AC_EXIT); break;
        case SDLK_AC_SAVE: m_store_keyboard_down_buffer(K_AC_SAVE); break;
        case SDLK_AC_PRINT: m_store_keyboard_down_buffer(K_AC_PRINT); break;
        case SDLK_AC_PROPERTIES: m_store_keyboard_down_buffer(K_AC_PROPERTIES); break;
        case SDLK_AC_SEARCH: m_store_keyboard_down_buffer(K_AC_SEARCH); break;
        case SDLK_AC_HOME: m_store_keyboard_down_buffer(K_AC_HOME); break;
        case SDLK_AC_BACK: m_store_keyboard_down_buffer(K_AC_BACK); break;
        case SDLK_AC_FORWARD: m_store_keyboard_down_buffer(K_AC_FORWARD); break;
        case SDLK_AC_STOP: m_store_keyboard_down_buffer(K_AC_STOP); break;
        case SDLK_AC_REFRESH: m_store_keyboard_down_buffer(K_AC_REFRESH); break;
        case SDLK_AC_BOOKMARKS: m_store_keyboard_down_buffer(K_AC_BOOKMARKS); break;
        case SDLK_SOFTLEFT: m_store_keyboard_down_buffer(K_SOFTLEFT); break;
        case SDLK_SOFTRIGHT: m_store_keyboard_down_buffer(K_SOFTRIGHT); break;
        case SDLK_CALL: m_store_keyboard_down_buffer(K_CALL); break;
        case SDLK_ENDCALL: m_store_keyboard_down_buffer(K_ENDCALL); break;
        case SDLK_LEFT_TAB: m_store_keyboard_down_buffer(K_LEFT_TAB); break;
        case SDLK_LEVEL5_SHIFT: m_store_keyboard_down_buffer(K_LEVEL5_SHIFT); break;
        case SDLK_MULTI_KEY_COMPOSE: m_store_keyboard_down_buffer(K_MULTI_KEY_COMPOSE); break;
        case SDLK_LMETA: m_store_keyboard_down_buffer(K_LMETA); break;
        case SDLK_RMETA: m_store_keyboard_down_buffer(K_RMETA); break;
        case SDLK_LHYPER: m_store_keyboard_down_buffer(K_LHYPER); break;
        case SDLK_RHYPER: m_store_keyboard_down_buffer(K_RHYPER); break;

        default: break;
    }
}
void EngineInputHub::m_redirect_keyboard_up_buffer(SDL_Keycode p_key){
    switch (p_key) {
        case SDLK_UNKNOWN: m_store_keyboard_up_buffer(K_UNKNOWN); break;
        case SDLK_RETURN: m_store_keyboard_up_buffer(K_RETURN); break;
        case SDLK_ESCAPE: m_store_keyboard_up_buffer(K_ESCAPE); break;
        case SDLK_BACKSPACE: m_store_keyboard_up_buffer(K_BACKSPACE); break;
        case SDLK_TAB: m_store_keyboard_up_buffer(K_TAB); break;
        case SDLK_SPACE: m_store_keyboard_up_buffer(K_SPACE); break;
        case SDLK_EXCLAIM: m_store_keyboard_up_buffer(K_EXCLAIM); break;
        case SDLK_DBLAPOSTROPHE: m_store_keyboard_up_buffer(K_DBLAPOSTROPHE); break;
        case SDLK_HASH: m_store_keyboard_up_buffer(K_HASH); break;
        case SDLK_DOLLAR: m_store_keyboard_up_buffer(K_DOLLAR); break;
        case SDLK_PERCENT: m_store_keyboard_up_buffer(K_PERCENT); break;
        case SDLK_AMPERSAND: m_store_keyboard_up_buffer(K_AMPERSAND); break;
        case SDLK_APOSTROPHE: m_store_keyboard_up_buffer(K_APOSTROPHE); break;
        case SDLK_LEFTPAREN: m_store_keyboard_up_buffer(K_LEFTPAREN); break;
        case SDLK_RIGHTPAREN: m_store_keyboard_up_buffer(K_RIGHTPAREN); break;
        case SDLK_ASTERISK: m_store_keyboard_up_buffer(K_ASTERISK); break;
        case SDLK_PLUS: m_store_keyboard_up_buffer(K_PLUS); break;
        case SDLK_COMMA: m_store_keyboard_up_buffer(K_COMMA); break;
        case SDLK_MINUS: m_store_keyboard_up_buffer(K_MINUS); break;
        case SDLK_PERIOD: m_store_keyboard_up_buffer(K_PERIOD); break;
        case SDLK_SLASH: m_store_keyboard_up_buffer(K_SLASH); break;
        case SDLK_0: m_store_keyboard_up_buffer(K_0); break;
        case SDLK_1: m_store_keyboard_up_buffer(K_1); break;
        case SDLK_2: m_store_keyboard_up_buffer(K_2); break;
        case SDLK_3: m_store_keyboard_up_buffer(K_3); break;
        case SDLK_4: m_store_keyboard_up_buffer(K_4); break;
        case SDLK_5: m_store_keyboard_up_buffer(K_5); break;
        case SDLK_6: m_store_keyboard_up_buffer(K_6); break;
        case SDLK_7: m_store_keyboard_up_buffer(K_7); break;
        case SDLK_8: m_store_keyboard_up_buffer(K_8); break;
        case SDLK_9: m_store_keyboard_up_buffer(K_9); break;
        case SDLK_COLON: m_store_keyboard_up_buffer(K_COLON); break;
        case SDLK_SEMICOLON: m_store_keyboard_up_buffer(K_SEMICOLON); break;
        case SDLK_LESS: m_store_keyboard_up_buffer(K_LESS); break;
        case SDLK_EQUALS: m_store_keyboard_up_buffer(K_EQUALS); break;
        case SDLK_GREATER: m_store_keyboard_up_buffer(K_GREATER); break;
        case SDLK_QUESTION: m_store_keyboard_up_buffer(K_QUESTION); break;
        case SDLK_AT: m_store_keyboard_up_buffer(K_AT); break;
        case SDLK_LEFTBRACKET: m_store_keyboard_up_buffer(K_LEFTBRACKET); break;
        case SDLK_BACKSLASH: m_store_keyboard_up_buffer(K_BACKSLASH); break;
        case SDLK_RIGHTBRACKET: m_store_keyboard_up_buffer(K_RIGHTBRACKET); break;
        case SDLK_CARET: m_store_keyboard_up_buffer(K_CARET); break;
        case SDLK_UNDERSCORE: m_store_keyboard_up_buffer(K_UNDERSCORE); break;
        case SDLK_GRAVE: m_store_keyboard_up_buffer(K_GRAVE); break;
        case SDLK_A: m_store_keyboard_up_buffer(K_A); break;
        case SDLK_B: m_store_keyboard_up_buffer(K_B); break;
        case SDLK_C: m_store_keyboard_up_buffer(K_C); break;
        case SDLK_D: m_store_keyboard_up_buffer(K_D); break;
        case SDLK_E: m_store_keyboard_up_buffer(K_E); break;
        case SDLK_F: m_store_keyboard_up_buffer(K_F); break;
        case SDLK_G: m_store_keyboard_up_buffer(K_G); break;
        case SDLK_H: m_store_keyboard_up_buffer(K_H); break;
        case SDLK_I: m_store_keyboard_up_buffer(K_I); break;
        case SDLK_J: m_store_keyboard_up_buffer(K_J); break;
        case SDLK_K: m_store_keyboard_up_buffer(K_K); break;
        case SDLK_L: m_store_keyboard_up_buffer(K_L); break;
        case SDLK_M: m_store_keyboard_up_buffer(K_M); break;
        case SDLK_N: m_store_keyboard_up_buffer(K_N); break;
        case SDLK_O: m_store_keyboard_up_buffer(K_O); break;
        case SDLK_P: m_store_keyboard_up_buffer(K_P); break;
        case SDLK_Q: m_store_keyboard_up_buffer(K_Q); break;
        case SDLK_R: m_store_keyboard_up_buffer(K_R); break;
        case SDLK_S: m_store_keyboard_up_buffer(K_S); break;
        case SDLK_T: m_store_keyboard_up_buffer(K_T); break;
        case SDLK_U: m_store_keyboard_up_buffer(K_U); break;
        case SDLK_V: m_store_keyboard_up_buffer(K_V); break;
        case SDLK_W: m_store_keyboard_up_buffer(K_W); break;
        case SDLK_X: m_store_keyboard_up_buffer(K_X); break;
        case SDLK_Y: m_store_keyboard_up_buffer(K_Y); break;
        case SDLK_Z: m_store_keyboard_up_buffer(K_Z); break;
        case SDLK_LEFTBRACE: m_store_keyboard_up_buffer(K_LEFTBRACE); break;
        case SDLK_PIPE: m_store_keyboard_up_buffer(K_PIPE); break;
        case SDLK_RIGHTBRACE: m_store_keyboard_up_buffer(K_RIGHTBRACE); break;
        case SDLK_TILDE: m_store_keyboard_up_buffer(K_TILDE); break;
        case SDLK_DELETE: m_store_keyboard_up_buffer(K_DELETE); break;
        case SDLK_PLUSMINUS: m_store_keyboard_up_buffer(K_PLUSMINUS); break;
        case SDLK_CAPSLOCK: m_store_keyboard_up_buffer(K_CAPSLOCK); break;
        case SDLK_F1: m_store_keyboard_up_buffer(K_F1); break;
        case SDLK_F2: m_store_keyboard_up_buffer(K_F2); break;
        case SDLK_F3: m_store_keyboard_up_buffer(K_F3); break;
        case SDLK_F4: m_store_keyboard_up_buffer(K_F4); break;
        case SDLK_F5: m_store_keyboard_up_buffer(K_F5); break;
        case SDLK_F6: m_store_keyboard_up_buffer(K_F6); break;
        case SDLK_F7: m_store_keyboard_up_buffer(K_F7); break;
        case SDLK_F8: m_store_keyboard_up_buffer(K_F8); break;
        case SDLK_F9: m_store_keyboard_up_buffer(K_F9); break;
        case SDLK_F10: m_store_keyboard_up_buffer(K_F10); break;
        case SDLK_F11: m_store_keyboard_up_buffer(K_F11); break;
        case SDLK_F12: m_store_keyboard_up_buffer(K_F12); break;
        case SDLK_PRINTSCREEN: m_store_keyboard_up_buffer(K_PRINTSCREEN); break;
        case SDLK_SCROLLLOCK: m_store_keyboard_up_buffer(K_SCROLLLOCK); break;
        case SDLK_PAUSE: m_store_keyboard_up_buffer(K_PAUSE); break;
        case SDLK_INSERT: m_store_keyboard_up_buffer(K_INSERT); break;
        case SDLK_HOME: m_store_keyboard_up_buffer(K_HOME); break;
        case SDLK_PAGEUP: m_store_keyboard_up_buffer(K_PAGEUP); break;
        case SDLK_END: m_store_keyboard_up_buffer(K_END); break;
        case SDLK_PAGEDOWN: m_store_keyboard_up_buffer(K_PAGEDOWN); break;
        case SDLK_RIGHT: m_store_keyboard_up_buffer(K_RIGHT); break;
        case SDLK_LEFT: m_store_keyboard_up_buffer(K_LEFT); break;
        case SDLK_DOWN: m_store_keyboard_up_buffer(K_DOWN); break;
        case SDLK_UP: m_store_keyboard_up_buffer(K_UP); break;
        case SDLK_NUMLOCKCLEAR: m_store_keyboard_up_buffer(K_NUMLOCKCLEAR); break;
        case SDLK_KP_DIVIDE: m_store_keyboard_up_buffer(K_KP_DIVIDE); break;
        case SDLK_KP_MULTIPLY: m_store_keyboard_up_buffer(K_KP_MULTIPLY); break;
        case SDLK_KP_MINUS: m_store_keyboard_up_buffer(K_KP_MINUS); break;
        case SDLK_KP_PLUS: m_store_keyboard_up_buffer(K_KP_PLUS); break;
        case SDLK_KP_ENTER: m_store_keyboard_up_buffer(K_KP_ENTER); break;
        case SDLK_KP_1: m_store_keyboard_up_buffer(K_KP_1); break;
        case SDLK_KP_2: m_store_keyboard_up_buffer(K_KP_2); break;
        case SDLK_KP_3: m_store_keyboard_up_buffer(K_KP_3); break;
        case SDLK_KP_4: m_store_keyboard_up_buffer(K_KP_4); break;
        case SDLK_KP_5: m_store_keyboard_up_buffer(K_KP_5); break;
        case SDLK_KP_6: m_store_keyboard_up_buffer(K_KP_6); break;
        case SDLK_KP_7: m_store_keyboard_up_buffer(K_KP_7); break;
        case SDLK_KP_8: m_store_keyboard_up_buffer(K_KP_8); break;
        case SDLK_KP_9: m_store_keyboard_up_buffer(K_KP_9); break;
        case SDLK_KP_0: m_store_keyboard_up_buffer(K_KP_0); break;
        case SDLK_KP_PERIOD: m_store_keyboard_up_buffer(K_KP_PERIOD); break;
        case SDLK_APPLICATION: m_store_keyboard_up_buffer(K_APPLICATION); break;
        case SDLK_POWER: m_store_keyboard_up_buffer(K_POWER); break;
        case SDLK_KP_EQUALS: m_store_keyboard_up_buffer(K_KP_EQUALS); break;
        case SDLK_F13: m_store_keyboard_up_buffer(K_F13); break;
        case SDLK_F14: m_store_keyboard_up_buffer(K_F14); break;
        case SDLK_F15: m_store_keyboard_up_buffer(K_F15); break;
        case SDLK_F16: m_store_keyboard_up_buffer(K_F16); break;
        case SDLK_F17: m_store_keyboard_up_buffer(K_F17); break;
        case SDLK_F18: m_store_keyboard_up_buffer(K_F18); break;
        case SDLK_F19: m_store_keyboard_up_buffer(K_F19); break;
        case SDLK_F20: m_store_keyboard_up_buffer(K_F20); break;
        case SDLK_F21: m_store_keyboard_up_buffer(K_F21); break;
        case SDLK_F22: m_store_keyboard_up_buffer(K_F22); break;
        case SDLK_F23: m_store_keyboard_up_buffer(K_F23); break;
        case SDLK_F24: m_store_keyboard_up_buffer(K_F24); break;
        case SDLK_EXECUTE: m_store_keyboard_up_buffer(K_EXECUTE); break;
        case SDLK_HELP: m_store_keyboard_up_buffer(K_HELP); break;
        case SDLK_MENU: m_store_keyboard_up_buffer(K_MENU); break;
        case SDLK_SELECT: m_store_keyboard_up_buffer(K_SELECT); break;
        case SDLK_STOP: m_store_keyboard_up_buffer(K_STOP); break;
        case SDLK_AGAIN: m_store_keyboard_up_buffer(K_AGAIN); break;
        case SDLK_UNDO: m_store_keyboard_up_buffer(K_UNDO); break;
        case SDLK_CUT: m_store_keyboard_up_buffer(K_CUT); break;
        case SDLK_COPY: m_store_keyboard_up_buffer(K_COPY); break;
        case SDLK_PASTE: m_store_keyboard_up_buffer(K_PASTE); break;
        case SDLK_FIND: m_store_keyboard_up_buffer(K_FIND); break;
        case SDLK_MUTE: m_store_keyboard_up_buffer(K_MUTE); break;
        case SDLK_VOLUMEUP: m_store_keyboard_up_buffer(K_VOLUMEUP); break;
        case SDLK_VOLUMEDOWN: m_store_keyboard_up_buffer(K_VOLUMEDOWN); break;
        case SDLK_KP_COMMA: m_store_keyboard_up_buffer(K_KP_COMMA); break;
        case SDLK_KP_EQUALSAS400: m_store_keyboard_up_buffer(K_KP_EQUALSAS400); break;
        case SDLK_ALTERASE: m_store_keyboard_up_buffer(K_ALTERASE); break;
        case SDLK_SYSREQ: m_store_keyboard_up_buffer(K_SYSREQ); break;
        case SDLK_CANCEL: m_store_keyboard_up_buffer(K_CANCEL); break;
        case SDLK_CLEAR: m_store_keyboard_up_buffer(K_CLEAR); break;
        case SDLK_PRIOR: m_store_keyboard_up_buffer(K_PRIOR); break;
        case SDLK_RETURN2: m_store_keyboard_up_buffer(K_RETURN2); break;
        case SDLK_SEPARATOR: m_store_keyboard_up_buffer(K_SEPARATOR); break;
        case SDLK_OUT: m_store_keyboard_up_buffer(K_OUT); break;
        case SDLK_OPER: m_store_keyboard_up_buffer(K_OPER); break;
        case SDLK_CLEARAGAIN: m_store_keyboard_up_buffer(K_CLEARAGAIN); break;
        case SDLK_CRSEL: m_store_keyboard_up_buffer(K_CRSEL); break;
        case SDLK_EXSEL: m_store_keyboard_up_buffer(K_EXSEL); break;
        case SDLK_KP_00: m_store_keyboard_up_buffer(K_KP_00); break;
        case SDLK_KP_000: m_store_keyboard_up_buffer(K_KP_000); break;
        case SDLK_THOUSANDSSEPARATOR: m_store_keyboard_up_buffer(K_THOUSANDSSEPARATOR); break;
        case SDLK_DECIMALSEPARATOR: m_store_keyboard_up_buffer(K_DECIMALSEPARATOR); break;
        case SDLK_CURRENCYUNIT: m_store_keyboard_up_buffer(K_CURRENCYUNIT); break;
        case SDLK_CURRENCYSUBUNIT: m_store_keyboard_up_buffer(K_CURRENCYSUBUNIT); break;
        case SDLK_KP_LEFTPAREN: m_store_keyboard_up_buffer(K_KP_LEFTPAREN); break;
        case SDLK_KP_RIGHTPAREN: m_store_keyboard_up_buffer(K_KP_RIGHTPAREN); break;
        case SDLK_KP_LEFTBRACE: m_store_keyboard_up_buffer(K_KP_LEFTBRACE); break;
        case SDLK_KP_RIGHTBRACE: m_store_keyboard_up_buffer(K_KP_RIGHTBRACE); break;
        case SDLK_KP_TAB: m_store_keyboard_up_buffer(K_KP_TAB); break;
        case SDLK_KP_BACKSPACE: m_store_keyboard_up_buffer(K_KP_BACKSPACE); break;
        case SDLK_KP_A: m_store_keyboard_up_buffer(K_KP_A); break;
        case SDLK_KP_B: m_store_keyboard_up_buffer(K_KP_B); break;
        case SDLK_KP_C: m_store_keyboard_up_buffer(K_KP_C); break;
        case SDLK_KP_D: m_store_keyboard_up_buffer(K_KP_D); break;
        case SDLK_KP_E: m_store_keyboard_up_buffer(K_KP_E); break;
        case SDLK_KP_F: m_store_keyboard_up_buffer(K_KP_F); break;
        case SDLK_KP_XOR: m_store_keyboard_up_buffer(K_KP_XOR); break;
        case SDLK_KP_POWER: m_store_keyboard_up_buffer(K_KP_POWER); break;
        case SDLK_KP_PERCENT: m_store_keyboard_up_buffer(K_KP_PERCENT); break;
        case SDLK_KP_LESS: m_store_keyboard_up_buffer(K_KP_LESS); break;
        case SDLK_KP_GREATER: m_store_keyboard_up_buffer(K_KP_GREATER); break;
        case SDLK_KP_AMPERSAND: m_store_keyboard_up_buffer(K_KP_AMPERSAND); break;
        case SDLK_KP_DBLAMPERSAND: m_store_keyboard_up_buffer(K_KP_DBLAMPERSAND); break;
        case SDLK_KP_VERTICALBAR: m_store_keyboard_up_buffer(K_KP_VERTICALBAR); break;
        case SDLK_KP_DBLVERTICALBAR: m_store_keyboard_up_buffer(K_KP_DBLVERTICALBAR); break;
        case SDLK_KP_COLON: m_store_keyboard_up_buffer(K_KP_COLON); break;
        case SDLK_KP_HASH: m_store_keyboard_up_buffer(K_KP_HASH); break;
        case SDLK_KP_SPACE: m_store_keyboard_up_buffer(K_KP_SPACE); break;
        case SDLK_KP_AT: m_store_keyboard_up_buffer(K_KP_AT); break;
        case SDLK_KP_EXCLAM: m_store_keyboard_up_buffer(K_KP_EXCLAM); break;
        case SDLK_KP_MEMSTORE: m_store_keyboard_up_buffer(K_KP_MEMSTORE); break;
        case SDLK_KP_MEMRECALL: m_store_keyboard_up_buffer(K_KP_MEMRECALL); break;
        case SDLK_KP_MEMCLEAR: m_store_keyboard_up_buffer(K_KP_MEMCLEAR); break;
        case SDLK_KP_MEMADD: m_store_keyboard_up_buffer(K_KP_MEMADD); break;
        case SDLK_KP_MEMSUBTRACT: m_store_keyboard_up_buffer(K_KP_MEMSUBTRACT); break;
        case SDLK_KP_MEMMULTIPLY: m_store_keyboard_up_buffer(K_KP_MEMMULTIPLY); break;
        case SDLK_KP_MEMDIVIDE: m_store_keyboard_up_buffer(K_KP_MEMDIVIDE); break;
        case SDLK_KP_PLUSMINUS: m_store_keyboard_up_buffer(K_KP_PLUSMINUS); break;
        case SDLK_KP_CLEAR: m_store_keyboard_up_buffer(K_KP_CLEAR); break;
        case SDLK_KP_CLEARENTRY: m_store_keyboard_up_buffer(K_KP_CLEARENTRY); break;
        case SDLK_KP_BINARY: m_store_keyboard_up_buffer(K_KP_BINARY); break;
        case SDLK_KP_OCTAL: m_store_keyboard_up_buffer(K_KP_OCTAL); break;
        case SDLK_KP_DECIMAL: m_store_keyboard_up_buffer(K_KP_DECIMAL); break;
        case SDLK_KP_HEXADECIMAL: m_store_keyboard_up_buffer(K_KP_HEXADECIMAL); break;
        case SDLK_LCTRL: m_store_keyboard_up_buffer(K_LCTRL); break;
        case SDLK_LSHIFT: m_store_keyboard_up_buffer(K_LSHIFT); break;
        case SDLK_LALT: m_store_keyboard_up_buffer(K_LALT); break;
        case SDLK_LGUI: m_store_keyboard_up_buffer(K_LGUI); break;
        case SDLK_RCTRL: m_store_keyboard_up_buffer(K_RCTRL); break;
        case SDLK_RSHIFT: m_store_keyboard_up_buffer(K_RSHIFT); break;
        case SDLK_RALT: m_store_keyboard_up_buffer(K_RALT); break;
        case SDLK_RGUI: m_store_keyboard_up_buffer(K_RGUI); break;
        case SDLK_MODE: m_store_keyboard_up_buffer(K_MODE); break;
        case SDLK_SLEEP: m_store_keyboard_up_buffer(K_SLEEP); break;
        case SDLK_WAKE: m_store_keyboard_up_buffer(K_WAKE); break;
        case SDLK_CHANNEL_INCREMENT: m_store_keyboard_up_buffer(K_CHANNEL_INCREMENT); break;
        case SDLK_CHANNEL_DECREMENT: m_store_keyboard_up_buffer(K_CHANNEL_DECREMENT); break;
        case SDLK_MEDIA_PLAY: m_store_keyboard_up_buffer(K_MEDIA_PLAY); break;
        case SDLK_MEDIA_PAUSE: m_store_keyboard_up_buffer(K_MEDIA_PAUSE); break;
        case SDLK_MEDIA_RECORD: m_store_keyboard_up_buffer(K_MEDIA_RECORD); break;
        case SDLK_MEDIA_FAST_FORWARD: m_store_keyboard_up_buffer(K_MEDIA_FAST_FORWARD); break;
        case SDLK_MEDIA_REWIND: m_store_keyboard_up_buffer(K_MEDIA_REWIND); break;
        case SDLK_MEDIA_NEXT_TRACK: m_store_keyboard_up_buffer(K_MEDIA_NEXT_TRACK); break;
        case SDLK_MEDIA_PREVIOUS_TRACK: m_store_keyboard_up_buffer(K_MEDIA_PREVIOUS_TRACK); break;
        case SDLK_MEDIA_STOP: m_store_keyboard_up_buffer(K_MEDIA_STOP); break;
        case SDLK_MEDIA_EJECT: m_store_keyboard_up_buffer(K_MEDIA_EJECT); break;
        case SDLK_MEDIA_PLAY_PAUSE: m_store_keyboard_up_buffer(K_MEDIA_PLAY_PAUSE); break;
        case SDLK_MEDIA_SELECT: m_store_keyboard_up_buffer(K_MEDIA_SELECT); break;
        case SDLK_AC_NEW: m_store_keyboard_up_buffer(K_AC_NEW); break;
        case SDLK_AC_OPEN: m_store_keyboard_up_buffer(K_AC_OPEN); break;
        case SDLK_AC_CLOSE: m_store_keyboard_up_buffer(K_AC_CLOSE); break;
        case SDLK_AC_EXIT: m_store_keyboard_up_buffer(K_AC_EXIT); break;
        case SDLK_AC_SAVE: m_store_keyboard_up_buffer(K_AC_SAVE); break;
        case SDLK_AC_PRINT: m_store_keyboard_up_buffer(K_AC_PRINT); break;
        case SDLK_AC_PROPERTIES: m_store_keyboard_up_buffer(K_AC_PROPERTIES); break;
        case SDLK_AC_SEARCH: m_store_keyboard_up_buffer(K_AC_SEARCH); break;
        case SDLK_AC_HOME: m_store_keyboard_up_buffer(K_AC_HOME); break;
        case SDLK_AC_BACK: m_store_keyboard_up_buffer(K_AC_BACK); break;
        case SDLK_AC_FORWARD: m_store_keyboard_up_buffer(K_AC_FORWARD); break;
        case SDLK_AC_STOP: m_store_keyboard_up_buffer(K_AC_STOP); break;
        case SDLK_AC_REFRESH: m_store_keyboard_up_buffer(K_AC_REFRESH); break;
        case SDLK_AC_BOOKMARKS: m_store_keyboard_up_buffer(K_AC_BOOKMARKS); break;
        case SDLK_SOFTLEFT: m_store_keyboard_up_buffer(K_SOFTLEFT); break;
        case SDLK_SOFTRIGHT: m_store_keyboard_up_buffer(K_SOFTRIGHT); break;
        case SDLK_CALL: m_store_keyboard_up_buffer(K_CALL); break;
        case SDLK_ENDCALL: m_store_keyboard_up_buffer(K_ENDCALL); break;
        case SDLK_LEFT_TAB: m_store_keyboard_up_buffer(K_LEFT_TAB); break;
        case SDLK_LEVEL5_SHIFT: m_store_keyboard_up_buffer(K_LEVEL5_SHIFT); break;
        case SDLK_MULTI_KEY_COMPOSE: m_store_keyboard_up_buffer(K_MULTI_KEY_COMPOSE); break;
        case SDLK_LMETA: m_store_keyboard_up_buffer(K_LMETA); break;
        case SDLK_RMETA: m_store_keyboard_up_buffer(K_RMETA); break;
        case SDLK_LHYPER: m_store_keyboard_up_buffer(K_LHYPER); break;
        case SDLK_RHYPER: m_store_keyboard_up_buffer(K_RHYPER); break;

        default: break;
    }
}
vec2 EngineInputHub::get_mouse_position(){
    return m_mouse_position;
}
vec2 EngineInputHub::get_mouse_world_position(){
    vec2 viewport_mouse_pos = GraphViewport::Ref()->screen_to_viewport(m_mouse_position);
    vec2 world_pos = GraphCamera::Ref()->viewport_to_world(viewport_mouse_pos);
    return world_pos;
}
vec2 EngineInputHub::get_mouse_wheel(){
    return m_mouse_wheel;
}
bool EngineInputHub::is_mouse_left_button_just_clicked(){
    return m_mouse_left_button_just_clicked;
}
bool EngineInputHub::is_mouse_right_button_just_clicked(){
    return m_mouse_right_button_just_clicked;
}
bool EngineInputHub::is_mouse_left_button_just_released(){
    return m_mouse_left_button_just_released;
}
bool EngineInputHub::is_mouse_right_button_just_released(){
    return m_mouse_right_button_just_released;
}
bool EngineInputHub::is_mouse_left_button_clicked(){
    return m_mouse_left_button_clicked;
}
bool EngineInputHub::is_mouse_right_button_clicked(){
    return m_mouse_right_button_clicked;
}


bool EngineInputHub::is_close_requested(){
    return m_is_close_requested;
}
Uint32 EngineInputHub::get_close_window_id(){
    return m_close_window_id;
}

bool EngineInputHub::keyboard_is_just_down(EngineKeycode p_key){
    return m_keyboard_just_down_buffer.contains(p_key);
}
bool EngineInputHub::keyboard_is_just_up(EngineKeycode p_key){
    return m_keyboard_just_up_buffer.contains(p_key);
}
bool EngineInputHub::keyboard_is_down(EngineKeycode p_key){
    return m_keyboard_down_buffer.contains(p_key);
}

vec2 EngineInputHub::get_mouse_motion(){
    return m_mouse_motion;
}

bool EngineInputHub::is_mouse_in_window(){
    return m_is_mouse_in_window;
}