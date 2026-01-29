#include "editor/components/messager.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/timer_server.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "theme/theme_loader.h"
#include <algorithm>


void EditorMessager::draw(){
    vec2 window_size = EngineWindow::Ref()->get_window_size();
    vec2 spacing = {-35.0f, 15.0f};
    vec2 block_size = {500.0f, 25.0f};

    vec2 pos = vec2(0.0f, window_size.y) - spacing - block_size.y;
    
    ImVec4 bg_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 text_color = ThemeLoader::Ref()->get_imgui_color("TextColour");
    const float basic_aph = 0.85f;
    const float basic_aph_dtl = TimeUnit(TimeUnit::Type::SECOND, 2.0f).get_delta();
    const float break_aph = 0.1;

    int i = 0;
    for(Message& msg : m_messages){
        if(msg.delta > basic_aph_dtl){
            float aph = (((m_max_dlt - basic_aph_dtl) - (msg.delta - basic_aph_dtl)) / (m_max_dlt - basic_aph_dtl)) * basic_aph;
            bg_color.w = aph;
            text_color.w = aph;
            if(aph < break_aph){
                msg.delta = std::numeric_limits<double>::max();
                break;
            }
        }else{
            bg_color.w = basic_aph;
            text_color.w = basic_aph;
        }

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ThemeLoader::Ref()->ImVec4_to_int(bg_color));
        ImGui::PushStyleColor(ImGuiCol_Border, ThemeLoader::Ref()->ImVec4_to_int(bg_color));
        ImGui::PushStyleColor(ImGuiCol_Text, ThemeLoader::Ref()->ImVec4_to_int(text_color));


        ImGui::SetNextWindowSize({block_size.x, block_size.y});
        ImGui::SetNextWindowPos({pos.x, pos.y});

        ImGui::Begin(msg.uid.c_str(), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        {
            std::string text = "[INFO] " + msg.content;

            
            ImGui::Text("%s", text.c_str());

            ImVec2 button_size = {15.0f, 17.0f};
            float button_spacing = 10.0f;
            ImGui::SetCursorPos({block_size.x - button_size.x - button_spacing, block_size.y / 2.0f - button_size.y / 2.0f});
            std::string button_id = "X##MSG_BUTTON_" + msg.uid;
            ImGui::Button(button_id.c_str(), button_size);
            if(ImGui::IsItemClicked()){
                msg.delta = std::numeric_limits<double>::max();
            }
        }
        m_is_messager_hovered = ImGui::IsWindowHovered();
        ImGui::End();
        
        pos.y -= (spacing.y + block_size.y);

        ImGui::PopStyleColor(3);

        i++;
    }

}

void EditorMessager::process(){
    m_update_msg_dlt();
    m_clear_msg();
    if(m_is_messager_hovered){
        EventLockedAll event1;
        EventMouseHoverObj event2;
        event2.hovering_pos = MouseServer::Ref()->get_mouse_world_position();

        EventServer::Ref()->emit(event1);
        EventServer::Ref()->emit(event2);
    }
}

void EditorMessager::m_update_msg_dlt(){
    double dlt = EngineWindow::Ref()->get_delta();
    for(auto& msg : m_messages){
        msg.delta += dlt;
    }
}

void EditorMessager::m_clear_msg(){
    m_messages.erase(std::remove_if(m_messages.begin(), m_messages.end(), [this](Message msg){
        return msg.delta >= this->m_max_dlt;
    }), m_messages.end());
}

void EditorMessager::add_message(std::string p_cnt){
    Message msg;
    msg.uid = "##" + RandomCode(25).get();
    msg.content = p_cnt;

    const int max_len = 55;
    if(msg.content.length() > max_len){
        msg.content = msg.content.substr(0, max_len) + "...";
    }

    msg.delta = 0;
    m_messages.push_back(msg);
}

void EditorMessager::init(){
    /*
    DEBUG_BIND_ACTION([this](std::string p_str){ this->add_message(p_str.replace(0, 51, "")); });
    INFO_BIND_ACTION([this](std::string p_str){ this->add_message(p_str.replace(0, 51, "")); });
    ERROR_BIND_ACTION([this](std::string p_str){ this->add_message(p_str.replace(0, 51, "")); });
    SUCCESS_BIND_ACTION([this](std::string p_str){ this->add_message(p_str.replace(0, 51, "")); });
    */
}