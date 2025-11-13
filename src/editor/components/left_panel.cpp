#include "editor/components/left_panel.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "SDL3/SDL_video.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "struct/shape/rect2.h"
#include <cfloat>

EditorLeftPanel::EditorLeftPanel(){
    BIND_CLASS(EditorLeftPanel);
    m_double_click_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::MILLISECOND, 200) ,true);
    m_double_click_timer->stop();
}
EditorLeftPanel::~EditorLeftPanel(){

}

void EditorLeftPanel::ready(){
    this->disable_cursor_change();
    this->set_hovering_type(Type::SCREEN);
}
void EditorLeftPanel::pre_process(){
    m_update_shape();

    Rect2 window_rect = EditorLayout::Ref()->covnert_to_window(*(Rect2*)m_space);
    bool is_display = window_rect.get_size().x > 0.0f;
    
    if(is_display){
        ImGui::SetNextWindowSize({window_rect.get_size().x, window_rect.get_size().y});
        ImGui::SetNextWindowPos({window_rect.get_left_top().x, window_rect.get_left_top().y});

        ImGuiStyle& style = ImGui::GetStyle();
        style.TabRounding = 0.1f;


        std::vector<std::string> items = {
            "Item 1", "Item 2", "Item 3", "Item 4", "Item 5",
            "Item 6", "Item 7", "Item 8", "Item 9", "Item 10",
            "Item 11", "Item 12", "Item 13", "Item 14", "Item 15",
            "Item 16", "Item 17", "Item 18", "Item 19", "Item 20"
        };

        ImGui::Begin("EditorLeftPanel", &is_display, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

        if (ImGui::BeginTabBar("TabBar")){
            if (ImGui::BeginTabItem("Inspector")){

                if (ImGui::BeginListBox("##MyListBox", ImVec2(-FLT_MIN, -FLT_MIN))){
                    for (int i = 0; i < items.size(); ++i){
                        const bool is_selected = (m_current_item == i);

                        if (ImGui::Selectable(items[i].c_str(), is_selected)){
                            if(m_current_item == i){
                                m_current_item = -1;
                            }else{
                                m_current_item = i;
                            }
                        }

                        if (is_selected){
                            double item_height = ImGui::GetTextLineHeightWithSpacing();
                            ImGui::PushStyleColor(ImGuiCol_FrameBg, ThemeLoader::Ref()->get_imgui_color("SecondaryColour3"));
                            if (ImGui::BeginListBox("##MyListBox1", ImVec2(-FLT_MIN, item_height * items.size() + 5))){
                                for (int i = 0; i < items.size(); ++i){
                                    if (ImGui::Selectable(("  "+items[i]).c_str(), false)){
                                        
                                    }
                                }
                                ImGui::EndListBox();
                            }
                            ImGui::PopStyleColor();
                        }
                    }

                    double item_height = ImGui::GetTextLineHeightWithSpacing();
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ThemeLoader::Ref()->get_imgui_color("SecondaryColour2"));
                    if (ImGui::BeginListBox("##MyListBox2", ImVec2(-FLT_MIN, item_height * items.size() + 5))){
                        for (int i = 0; i < items.size(); ++i){
                            if (ImGui::Selectable(("*"+items[i]).c_str(), false)){
                                
                            }
                        }
                        ImGui::EndListBox();
                    }
                    ImGui::PopStyleColor();

                    ImGui::EndListBox();
                }


                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
void EditorLeftPanel::process(){
}
void EditorLeftPanel::post_process(){

}
void EditorLeftPanel::draw(){

}


void EditorLeftPanel::ui_init(EditorSpace* p_space){
    m_space = p_space;
}

void EditorLeftPanel::m_update_shape(){
    m_shape = *(Rect2*)m_space;
    this->set_shape(m_shape);
}