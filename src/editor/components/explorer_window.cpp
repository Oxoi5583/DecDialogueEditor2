#include "editor/components/explorer_window.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/font_loader.h"
#include "engine/window.h"
#include "glm/ext/vector_float2.hpp"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>

#include "ext/debug/messenger_ext.h"
#include "server/ui_icon_unicode.h"

ExplorerWindow::ExplorerWindow(){
    BIND_CLASS(ExplorerWindow);
}
ExplorerWindow::~ExplorerWindow(){

}

void ExplorerWindow::ready(){

}
void ExplorerWindow::m_close_button_process(){
    float content_width = ImGui::GetContentRegionAvail().x + 10;
    float button_width = 20.0f;
    float button_height = 20.0f;
    float button_x = content_width - button_width;
    ImGui::SetCursorPosX(button_x);

    std::string button_id = ICON_WIN_CLOSE;
    button_id += "##";
    button_id += m_uid;

    ImGui::PushFont(EngineFontLoader::Ref()->get(12));
    if(ImGui::Button(button_id.c_str(), ImVec2(button_width, button_height))){
        this->queue_free();
    }
    ImGui::PopFont();
    if(ImGui::IsItemHovered()){
        {
            EventMouseHoverObj event;
            event.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
            event.is_pointer_cursor = false;
            event.obj_id = this->get_id();
            EventServer::Ref()->emit(event);
        }
        {
            EventLockedAll event;
            EventServer::Ref()->emit(event);
        }
    }
}
void ExplorerWindow::pre_process(){
    vec2 engine_win_size = EngineWindow::Ref()->get_window_size();
    vec2 win_size = {engine_win_size.x * 0.75f, engine_win_size.y * 0.65f};
    ImGui::SetNextWindowPos({engine_win_size.x / 2.0f - win_size.x / 2.0f,
                                engine_win_size.y / 2.0f - win_size.y / 2.0f});
    ImGui::SetNextWindowSize({win_size.x, win_size.y});
    ImGui::Begin("##EXPLORER_WINDOW", NULL, ImGuiWindowFlags_NoDecoration);
        m_close_button_process();

        if(ImGui::IsWindowHovered()){
            {
                EventMouseHoverObj evt;
                evt.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
                evt.obj_id = this->get_id();
                EventServer::Ref()->emit(evt);
            }
            {
                EventLockedAll evt;
                EventServer::Ref()->emit(evt);
            }
        }
    ImGui::End();
}
void ExplorerWindow::process(){

}
void ExplorerWindow::post_process(){

}
void ExplorerWindow::draw(){

}
