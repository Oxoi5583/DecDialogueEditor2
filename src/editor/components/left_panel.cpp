#include "editor/components/left_panel.h"
#include "DecToolsBox/struct/roman_numeral.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
#include "graph/camera.h"
#include "nlohmann/json.hpp"
#include "obj/abstract/movable.h"
#include "obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "SDL3/SDL_video.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "struct/shape/rect2.h"
#include <cfloat>
#include <string>

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

        m_panel_data = GraphManager::Ref()->get_panel_data();

        ImGui::Begin("EditorLeftPanel", &is_display, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        if (ImGui::BeginTabBar("TabBar")){
            m_update_inpector();
            

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

void EditorLeftPanel::m_go_to_item(OID p_id){
    if(!ObjectServer::Ref()->is_id_valid(p_id)){
        return;
    }

    MovableObject* obj = ObjectServer::Ref()->get_instance<MovableObject>(p_id);
    if(!obj){
        return;
    }

    vec2 pos = obj->get_position();
    GraphCamera::Ref()->set_target(pos);
}

void EditorLeftPanel::m_update_inpector(){
    if (ImGui::BeginTabItem("Inspector")){
        if (ImGui::BeginListBox("##InspectorPriListBox", ImVec2(-FLT_MIN, -FLT_MIN))){
            m_update_inpector_primary_list();
            m_update_inpector_other_list();


            ImGui::EndListBox();
        }
        ImGui::EndTabItem();
    }
}

void EditorLeftPanel::m_update_inpector_primary_list(){
    auto pri_item_list = m_panel_data.primary_info_list;
    int pri_item_size = pri_item_list.size();
    for (int p = 0; p < pri_item_list.size(); ++p){
        auto pri_id = pri_item_list[p].id;
        auto pri_type = pri_item_list[p].type;
        auto pri_name = std::to_string(p + 1) + " - " + pri_item_list[p].name;
        auto pri_content = pri_item_list[p].content;

        const bool is_selected = (m_current_item == p);

        if (ImGui::Selectable(pri_name.c_str(), is_selected)){
            if(m_current_item == p){
                m_current_item = -1;
            }else{
                m_current_item = p;
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)){
            m_go_to_item(pri_id);
        }

        if (is_selected > 0){
            m_update_inpector_secondary_list(p);
        }
    }
}

void EditorLeftPanel::m_update_inpector_secondary_list(int p_parent_index){
    auto sec_item_list = m_panel_data.secondary_info_list[p_parent_index];
    const int sec_item_size = sec_item_list.size();
    if (sec_item_size > 0){
        const char* sec_list_box_name = "##InspectorSecListBox";
        const ImVec4 sec_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");

        const double sec_item_height = ImGui::GetTextLineHeightWithSpacing();
        const double sec_list_area_height = (sec_item_size == 0) ? 0.0f : sec_item_height * sec_item_size + 5;
        const ImVec2 sec_list_area = ImVec2(-FLT_MIN, sec_list_area_height);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, sec_list_box_color);
        if (ImGui::BeginListBox(sec_list_box_name, sec_list_area)){
            for (int s = 0; s < sec_item_size; ++s){
                auto sec_id = sec_item_list[s].id;
                auto sec_type = sec_item_list[s].type;
                auto sec_name = int_to_roman(s + 1) + " - " + sec_item_list[s].name;
                auto sec_content = sec_item_list[s].content;

                ImGui::Selectable(("  "+sec_name).c_str(), false);
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)){
                    m_go_to_item(sec_id);
                }
            }
            ImGui::EndListBox();
        }
        ImGui::PopStyleColor();
    }
}

void EditorLeftPanel::m_update_inpector_other_list(){
    auto other_item_list = m_panel_data.other_info_list;
    const int other_item_size = other_item_list.size();
    if (other_item_size > 0){
        const char* other_list_box_name = "##InspectorOtherListBox";
        const ImVec4 other_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");

        const double other_item_height = ImGui::GetTextLineHeightWithSpacing();
        const double other_list_area_height = (other_item_size == 0) ? 0.0f : other_item_height * other_item_size + 5;
        const ImVec2 other_list_area = ImVec2(-FLT_MIN, other_list_area_height);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, other_list_box_color);
        if (ImGui::BeginListBox(other_list_box_name, other_list_area)){
            for (int o = 0; o < other_item_size; ++o){
                auto other_id = other_item_list[o].id;
                auto other_type = other_item_list[o].type;
                auto other_name = std::to_string(o + 1) + " - " + other_item_list[o].name;
                auto other_content = other_item_list[o].content;

                ImGui::Selectable(("*"+other_name).c_str(), false);
            
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)){
                    m_go_to_item(other_id);
                }
            }
            ImGui::EndListBox();
        }
        ImGui::PopStyleColor();
    }
}

void EditorLeftPanel::ui_init(EditorSpace* p_space){
    m_space = p_space;
}

void EditorLeftPanel::m_update_shape(){
    m_shape = *(Rect2*)m_space;
    this->set_shape(m_shape);
}