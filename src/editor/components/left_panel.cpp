#include "editor/components/left_panel.h"
#include "DecToolsBox/struct/roman_numeral.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
#include "engine/input_hub.h"
#include "engine/input_key.h"
#include "graph/camera.h"
#include "graph/selection.h"
#include "nlohmann/json.hpp"
#include "obj/abstract/movable.h"
#include "obj/abstract/selectable.h"
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
#include "theme/theme_loader.h"
#include <cfloat>
#include <string>

EditorLeftPanel::EditorLeftPanel(){
    BIND_CLASS(EditorLeftPanel);
    m_double_click_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::MILLISECOND, 100) ,true);
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
    m_refresh_buffers();
    m_refresh_panel_data();
    m_update_penel_properties();

    if(m_is_displaying){
        ImGuiStyle& style = ImGui::GetStyle();
        style.TabRounding = 0.1f;

        m_panel_data = GraphManager::Ref()->get_panel_data();

        ImGui::Begin("EditorLeftPanel", &m_is_displaying, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        
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
    GraphCamera::Ref()->set_zoom(0.8f);
}

void EditorLeftPanel::m_update_inpector(){
    m_index = 0;
    std::vector<OID>().swap(m_iterated_ids);

    if (ImGui::BeginTabItem("Inspector")){
        ImGuiStyle& style = ImGui::GetStyle();
        if (ImGui::BeginListBox("##InspectorPriListBox", ImVec2(-FLT_MIN, -FLT_MIN))){
            m_update_inpector_primary_list();
            m_update_inpector_other_list();
            m_update_range_select();
            m_emit_event();


            ImGui::EndListBox();
        }
        ImGui::EndTabItem();
    }
    
    if (ImGui::BeginTabItem("Test")){
        ImGui::EndTabItem();
    }
}

void EditorLeftPanel::m_update_inpector_primary_list(){
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const double pri_item_height = ImGui::GetTextLineHeightWithSpacing();
    auto pri_item_list = m_panel_data.primary_info_list;
    int pri_item_size = pri_item_list.size();
    for (int p = 0; p < pri_item_list.size(); ++p){
        auto pri_id = pri_item_list[p].id;

        const bool is_selected = (GraphSelection::Ref()->is_id_in_dragging_buffer(pri_id)) ? true : pri_item_list[p].is_selected;

        auto pri_type = pri_item_list[p].type;
        std::string padding = (is_selected) ? " " : "";
        auto pri_name = padding + std::to_string(p + 1) + " - " + pri_item_list[p].name;
        auto pri_content = pri_item_list[p].content;

        const ImVec4 pri_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
        const ImVec4 pri_list_box_selected_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
        const ImVec4 pri_list_box_text_color = (is_selected) ? ThemeLoader::Ref()->get_imgui_color("HighlightTextColour") 
                                                                : ThemeLoader::Ref()->get_imgui_color("TextColour");

        ImGui::PushStyleColor(ImGuiCol_FrameBg, pri_list_box_color);
        ImGui::PushStyleColor(ImGuiCol_Header, pri_list_box_selected_color);
        ImGui::PushStyleColor(ImGuiCol_Text, pri_list_box_text_color);

        ImGui::Selectable((pri_name).c_str(), is_selected);

        m_update_item_status(pri_id);


        if (is_selected){
            ImVec2 p_min = ImGui::GetItemRectMin();
            ImVec2 p_max = {ImGui::GetItemRectMin().x + m_hint_width, ImGui::GetItemRectMax().y};

            ImGui::GetWindowDrawList()->AddRectFilled(
                p_min, p_max,
                ThemeLoader::Ref()->get_imgui_color_int("AccentColour1"),
                0.0f,
                0
            );

            m_update_inpector_secondary_list(p);
        }
        ImGui::PopStyleColor(3);

        m_iterated_ids.push_back(pri_id);
        m_index++;
    }
}

void EditorLeftPanel::m_update_inpector_secondary_list(int p_parent_index){
    auto& sec_item_list = m_panel_data.secondary_info_list[p_parent_index];
    const int sec_item_size = sec_item_list.size();
    if (sec_item_size > 0){
        const char* sec_list_box_name = "##InspectorSecListBox";

        const double sec_item_height = ImGui::GetTextLineHeightWithSpacing();
        const double sec_list_area_height = (sec_item_size == 0) ? 0.0f : sec_item_height * sec_item_size + 5;
        const ImVec2 sec_list_area = ImVec2(-FLT_MIN, sec_list_area_height);


                                                                
        if (ImGui::BeginListBox(sec_list_box_name, sec_list_area)){
            for (int s = 0; s < sec_item_size; ++s){
                auto sec_id = sec_item_list[s].id;

                const bool is_selected = (GraphSelection::Ref()->is_id_in_dragging_buffer(sec_id)) ? true :  sec_item_list[s].is_selected;

                auto sec_type = sec_item_list[s].type;
                std::string padding = (is_selected) ? " " : "";
                auto sec_name = padding + int_to_roman(s + 1) + " - " + sec_item_list[s].name;
                auto sec_content = sec_item_list[s].content;

                const ImVec4 sec_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
                const ImVec4 sec_list_box_selected_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
                const ImVec4 sec_list_box_text_color = (is_selected) ? ThemeLoader::Ref()->get_imgui_color("HighlightTextColour") 
                                                                        : ThemeLoader::Ref()->get_imgui_color("TextColour");

                ImGui::PushStyleColor(ImGuiCol_FrameBg, sec_list_box_color);
                ImGui::PushStyleColor(ImGuiCol_Header, sec_list_box_selected_color);
                ImGui::PushStyleColor(ImGuiCol_Text, sec_list_box_text_color);


                ImGui::Selectable(("  "+sec_name).c_str(), is_selected);

                m_update_item_status(sec_id);


                if (is_selected){
                    ImVec2 p_min = ImGui::GetItemRectMin();
                    ImVec2 p_max = {ImGui::GetItemRectMin().x + m_hint_width, ImGui::GetItemRectMax().y};

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        p_min, p_max,
                        ThemeLoader::Ref()->get_imgui_color_int("AccentColour1"),
                        0.0f,
                        0
                    );
                }

                ImGui::PopStyleVar();

                m_iterated_ids.push_back(sec_id);
                m_index++;
            }
            ImGui::EndListBox();
        }
    }
}

void EditorLeftPanel::m_update_inpector_other_list(){
    auto other_item_list = m_panel_data.other_info_list;
    const int other_item_size = other_item_list.size();
    if (other_item_size > 0){
        const char* other_list_box_name = "##InspectorOtherListBox";
        const double other_item_height = ImGui::GetTextLineHeightWithSpacing();
        const double other_list_area_height = (other_item_size == 0) ? 0.0f : other_item_height * other_item_size + 2;
        const ImVec2 other_list_area = ImVec2(-FLT_MIN, other_list_area_height);

        if (ImGui::BeginListBox(other_list_box_name, other_list_area)){
            for (int o = 0; o < other_item_size; ++o){
                auto other_id = other_item_list[o].id;

                const bool is_selected = (GraphSelection::Ref()->is_id_in_dragging_buffer(other_id)) ? true :  other_item_list[o].is_selected;

                auto other_type = other_item_list[o].type;
                std::string padding = (is_selected) ? " " : "";
                auto other_name = padding + "*" + std::to_string(o + 1) + " - " + other_item_list[o].name;
                auto other_content = other_item_list[o].content;

                const ImVec4 other_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
                const ImVec4 other_list_box_selected_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
                const ImVec4 other_list_box_text_color = (is_selected) ? ThemeLoader::Ref()->get_imgui_color("HighlightTextColour") 
                                                                        : ThemeLoader::Ref()->get_imgui_color("TextColour");

                ImGui::PushStyleColor(ImGuiCol_FrameBg, other_list_box_color);
                ImGui::PushStyleColor(ImGuiCol_Header, other_list_box_selected_color);
                ImGui::PushStyleColor(ImGuiCol_Text, other_list_box_text_color);


                ImGui::Selectable((other_name).c_str(), is_selected);

                m_update_item_status(other_id);

                if (is_selected){
                    ImVec2 p_min = ImGui::GetItemRectMin();
                    ImVec2 p_max = {ImGui::GetItemRectMin().x + m_hint_width, ImGui::GetItemRectMax().y};

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        p_min, p_max,
                        ThemeLoader::Ref()->get_imgui_color_int("AccentColour1"),
                        0.0f,
                        0
                    );
                }

                ImGui::PopStyleColor(3);

                m_iterated_ids.push_back(other_id);
                m_index++;
            }
            ImGui::EndListBox();
        }
    }
}

void EditorLeftPanel::ui_init(EditorSpace* p_space){
    m_space = p_space;
}

void EditorLeftPanel::m_update_shape(){
    m_shape = *(Rect2*)m_space;
    this->set_shape(m_shape);
}

void EditorLeftPanel::m_store_last_click_id(OID p_id){
    m_last_2_click_id = m_last_1_click_id;
    m_last_1_click_id = p_id;
}

void EditorLeftPanel::m_update_range_select(){
    if(!m_is_select_range){
        return;
    }
    
    GraphSelection::Ref()->unselect_all();

    bool is_in_range = false;

    for(OID& id : m_iterated_ids){
        if(id == m_last_1_click_id || id == m_last_2_click_id){
            if(ObjectServer::Ref()->is_id_valid(id)){
                ObjectServer::Ref()->get_instance<SelectableObject>(id)->select();
            }
            is_in_range = (is_in_range) ? false : true;
        }

        if(is_in_range){
            if(ObjectServer::Ref()->is_id_valid(id)){
                ObjectServer::Ref()->get_instance<SelectableObject>(id)->select();
            }
        }
    }

    m_is_select_range = false;
}

void EditorLeftPanel::m_update_item_status(OID& p_id){
    bool is_id_valid = ObjectServer::Ref()->is_id_valid(p_id);
    bool is_obj_selected = (is_id_valid) ? ObjectServer::Ref()->get_instance<SelectableObject>(p_id)->is_selected() : false;

    if(ImGui::IsItemHovered()){
        if(is_id_valid && is_obj_selected){
            EventLeftPanelSelectedItemHovered event;
            event.ids = GraphSelection::Ref()->get_selected();
            EventServer::Ref()->emit(event);
        }
        if(is_id_valid && !is_obj_selected){
            EventLeftPanelItemHovered event;
            event.id = p_id;
            EventServer::Ref()->emit(event);
        }
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)){
        m_go_to_item(p_id);
    }

    if (ImGui::IsItemClicked()){
        m_store_last_click_id(p_id);

        if(EngineInputHub::Ref()->keyboard_is_down(K_LCTRL)){
            if(is_id_valid){
                if(is_obj_selected){
                    ObjectServer::Ref()->get_instance<SelectableObject>(p_id)->unselect();
                }else{
                    ObjectServer::Ref()->get_instance<SelectableObject>(p_id)->select();
                }
            }
        }else{
            if(is_id_valid){
                GraphSelection::Ref()->unselect_all();
                ObjectServer::Ref()->get_instance<SelectableObject>(p_id)->select();
            }
        }

        
        if(EngineInputHub::Ref()->keyboard_is_down(K_LSHIFT)){
            m_is_select_range = true;
        }
    }

}

void EditorLeftPanel::m_refresh_buffers(){
    m_window_rect = EditorLayout::Ref()->covnert_to_window(*(Rect2*)m_space);
    m_is_displaying = m_window_rect.get_size().x > 0.0f;
}

void EditorLeftPanel::m_refresh_panel_data(){
    if(!m_double_click_timer->timeout_and_reset()){
        return;
    }

    if(GraphSelection::Ref()->is_group_dragging()){
        return;
    }
    if(!m_is_displaying){
        return;
    }
    m_panel_data = GraphManager::Ref()->get_panel_data();
}
void EditorLeftPanel::m_update_penel_properties(){
    if(!m_is_displaying){
        return;
    }
    ImGui::SetNextWindowSize({m_window_rect.get_size().x, m_window_rect.get_size().y});
    ImGui::SetNextWindowPos({m_window_rect.get_left_top().x, m_window_rect.get_left_top().y});
}

void EditorLeftPanel::m_emit_event(){
    if(this->was_hovered()){
        EventLeftPanelHovered event;
        EventServer::Ref()->emit(event);
    }
}