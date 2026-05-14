#include "editor/components/quick_text_display.h"
#include "server/ui_text_bank.h"
#include "engine/font_loader.h"
#include "imgui/imgui.h"
#include "server/mouse_server.h"
#include "theme/theme_loader.h"
#include <cstddef>

QuickTextDisplay::QuickTextDisplay(){}
QuickTextDisplay::~QuickTextDisplay(){}

void QuickTextDisplay::pre_process(){
    m_font_size = FONT_SIZE_MIDDLE;
    m_is_shown = false;
}
void QuickTextDisplay::process(){
    if(m_is_shown && m_text.length() > 0){
        ImGui::PushFont(EngineFontLoader::Ref()->get(m_font_size));

        ImVec4 color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
        color = ImVec4(color.x , color.y , color.z , color.w * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, color);

        vec2 pos = MouseServer::Ref()->get_mouse_screen_position();
        pos += vec2(15.0f, 15.0f);
        ImGui::SetNextWindowPos({pos.x,pos.y});

        ImGui::Begin("Quick Text Display", &m_is_shown, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoInputs);


        std::string display_text = "";
        
        size_t i = 0;
        for(char c : m_text){
            i++;
            display_text += c;
            
            if(c == '\n'){ 
                i = 0;
                continue;
            }

            if(i % m_max_display_length == 0){
                display_text += '\n';
            }
        }
        /*
        if(display_text.length() > m_max_display_length){
            display_text = display_text.substr(0, m_max_display_length) + "...";
        }
        */
        ImGui::TextUnformatted(display_text.c_str());

        ImGui::End();      
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }
}

void QuickTextDisplay::show(){
    m_is_shown = true;
}
void QuickTextDisplay::hide(){
    m_is_shown = false;
}
void QuickTextDisplay::set_text(std::string p_text){
    m_text = p_text;
}
void QuickTextDisplay::set_font_size(float p_size){
    m_font_size = p_size;
}