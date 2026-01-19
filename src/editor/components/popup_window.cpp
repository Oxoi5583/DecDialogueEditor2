#include "editor/components/popup_window.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_server.h"

PopupWindow::PopupWindow(){
    BIND_CLASS(PopupWindow);
}
PopupWindow::~PopupWindow(){

}

void PopupWindow::ready(){
}
void PopupWindow::pre_process(){
}
void PopupWindow::process(){
    if(m_is_first_process){
        glm::vec2 pos = (EngineWindow::Ref()->get_window_size() / 2.0f) - (m_size / 2.0f);
        ImGui::SetNextWindowPos({pos.x, pos.y});
        m_is_first_process = false;
    }
    ImGui::SetNextWindowSize({m_size.x, m_size.y});
    if(ImGui::Begin(m_uid.c_str(),&m_show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration)){
        for(auto it : m_options){
            if(ImGui::Button(it.first.c_str())){
                it.second();
            }
        }
    }


    if(ImGui::IsItemHovered()){
        EventMouseHoverObj event = EventMouseHoverObj();
        event.hovering_pos = MouseServer::Ref()->get_mouse_screen_position();
        event.is_pointer_cursor = false;
        event.obj_id = this->get_id();
        EventServer::Ref()->emit(event);
    }
    ImGui::End();
}
void PopupWindow::post_process(){
}
void PopupWindow::draw(){
}

void PopupWindow::set_title(std::string p_title){
    m_title = p_title;
}
void PopupWindow::set_content(std::string p_content){
    m_content = p_content;
}
void PopupWindow::add_option(std::string p_option_name, std::function<void()> p_action){
    m_options.try_emplace(p_option_name, p_action);
}
void PopupWindow::set_size(glm::vec2 p_size){
    m_size = p_size;
}


void PopupWindow::show(){
    m_show = true;
}
void PopupWindow::hide(){
    m_show = false;
}
void PopupWindow::close(){
    this->queue_free();
}