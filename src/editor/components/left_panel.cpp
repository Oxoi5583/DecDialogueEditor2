#include "editor/components/left_panel.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/core/roman_numeral.h"
#include "server/ui_icon_unicode.h"
#include "server/ui_text_bank.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
#include "engine/input_hub.h"
#include "engine/input_key.h"
#include "glm/ext/vector_float2.hpp"
#include "system/graph/camera.h"
#include "system/graph/selection.h"
#include "nlohmann/json.hpp"
#include "system/obj/abstract/movable.h"
#include "system/obj/abstract/selectable.h"
#include "system/obj/graph/entry.h"
#include "system/obj/graph/manager.h"
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
#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "editor/components/quick_text_display.h"

struct MsgGenerator{
    std::stringstream msg;
    void add_row(std::string p_field, std::string p_content){
        const int spacing = 10;
        msg << p_field;
        for(size_t i = 0; i < (spacing - p_field.size()); i++){
            msg << " ";
        }
        msg << " : " << p_content << std::endl;
    }
    std::string get(){
        return msg.str();
    }
};

EditorLeftPanel::EditorLeftPanel(){
    BIND_CLASS(EditorLeftPanel);
    m_double_click_timer.stop();
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
    m_update_penel_properties();

    if(m_is_displaying){
        ImGuiStyle& style = ImGui::GetStyle();
        style.TabRounding = 0.1f;

        ImGui::Begin("EditorLeftPanel", &m_is_displaying, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
        
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

    if (ImGui::BeginTabItem(UiTextBank::Ref()->Inspector)){
        ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_MIDDLE));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 3.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f,0.0f));
        ImGuiStyle& style = ImGui::GetStyle();
        if (ImGui::BeginListBox("##InspectorPriListBox", ImVec2(-FLT_MIN, -FLT_MIN))){
            m_update_inpector_primary_list();
            m_update_inpector_other_list();
            m_update_range_select();
            m_emit_event();


            ImGui::EndListBox();
        }
        ImGui::EndTabItem();
        ImGui::PopFont();
        ImGui::PopStyleVar(5);
    }
    
    if (ImGui::BeginTabItem(UiTextBank::Ref()->Explorer)){
        ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_MIDDLE));
        m_update_explorer_list();
            

        ImGui::EndTabItem();
        ImGui::PopFont();
    }
    
}

#include "server/file_server.h"
#include "server/project_server.h"

void EditorLeftPanel::m_update_explorer_list(){
    ImVec2 imgui_area = ImGui::GetWindowSize();
    vec2 button_start = {imgui_area.x, 40.0f};
    button_start += vec2(-25.0f, 0.0f);
    vec2 button_size = {20.0f, 20.0f};

    std::string button_id = ICON_FILE_ADD;
    button_id += "##ADD_NEW_FILE_BUTTON";

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f,0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f,0.0f));
    ImGui::SetCursorPos({10.0f, button_start.y});
    ImGui::TextUnformatted(UiTextBank::Ref()->WorkspaceTitle);
    ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_SMALL));
    ImGui::PushStyleColor(ImGuiCol_Button, ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour1"));
    ImGui::SameLine(button_start.x);
    ImGui::Button(button_id.c_str(), {button_size.x, button_size.y});
    if(ImGui::IsItemClicked()){
        ProjectServer::Ref()->create_workspace();
    }
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::PopStyleVar(2);

    if (ImGui::BeginListBox("##ExplorerListBox", ImVec2(-FLT_MIN, -FLT_MIN))){
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        vec2 min =  vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
        vec2 max = min + vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
        draw_list->AddRectFilled({min.x, min.y}, {max.x, max.y}, ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour3"));

        ImVec2 each_size = {ImGui::GetWindowContentRegionMax().x, ImGui::CalcTextSize("@").y};


        auto proj_data_v = ProjectServer::Ref()->get_display_data();

        ImVec4 color_selected = ThemeLoader::Ref()->get_imgui_color("AccentColour1");
        color_selected.w *= 0.65;
        ImU32 bg_selected = ThemeLoader::Ref()->ImVec4_to_int(color_selected);
        ImVec4 bg_non_selected = {0.0f,0.0f,0.0f,0.0f};

        for(auto& space : proj_data_v){
            std::string obj_id = space.name + "##" + space.uid;
            if(!space.is_saved){
                obj_id = "*" + obj_id;
            }
            
            int pop_times;
            if(space.is_selected){
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, bg_selected);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_selected);
                ImGui::PushStyleColor(ImGuiCol_Header, bg_selected);
                pop_times = 3;
            }else{
                ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_non_selected);
                ImGui::PushStyleColor(ImGuiCol_Header, bg_non_selected);
                pop_times = 2;
            }
            ImGui::Selectable(obj_id.c_str(), space.is_selected, 0, each_size);
            ImGui::PopStyleColor(pop_times);

            if(ImGui::IsItemHovered()){
                MsgGenerator gen;
                gen.add_row("Name", space.name);
                gen.add_row("Path", space.path);
                gen.add_row("Size", space.size.to_string());

                QuickTextDisplay::Ref()->set_text(gen.get());
                QuickTextDisplay::Ref()->set_font_size(13.0f);
                QuickTextDisplay::Ref()->show();

                EventHoveredExplorerList evt;
                evt.workspace_uid = space.uid;
                EventServer::Ref()->emit(evt);
                EventMouseHoverObj evt2;
                EventServer::Ref()->emit(evt2);
            }

            if(ImGui::IsItemClicked()){
                ProjectServer::Ref()->go_to_workspace(space.uid);
            }
        }
        ImGui::EndListBox();
    }
}

void EditorLeftPanel::m_update_inpector_primary_list(){
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const float item_height = ImGui::CalcTextSize("@").y + ImGui::GetStyle().ItemInnerSpacing.y * 2.0f;
    inpector_item_size = {0.0f, item_height};

    auto& pri_item_list = GraphManager::Ref()->get_panel_data().primary_info_list;

    const ImVec4 pri_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    const ImVec4 pri_list_box_selected_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    const ImVec4 button_color = {0.0f, 0.0f, 0.0f, 0.0f};
    const ImU32 sub_area_color = ThemeLoader::Ref()->get_imgui_color_int("AccentColour1");
    const ImU32 break_color = ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour3");

    for (int p = 0; p < pri_item_list.size(); ++p){
        OID pri_id = pri_item_list[p];
        const GraphManager::NodeInfo& node_info = GraphManager::Ref()->get_node_info(pri_id);

        const bool is_selected = (GraphSelection::Ref()->is_id_in_dragging_buffer(pri_id)) ? true : node_info.is_selected;

        std::string padding = (is_selected) ? "    " : "";
        auto pri_name = padding + std::to_string(p + 1) + " - " + node_info.name;

        ImGui::PushStyleColor(ImGuiCol_FrameBg, pri_list_box_color);
        ImGui::PushStyleColor(ImGuiCol_Header, pri_list_box_selected_color);
        bool is_expanded = node_info.is_expanded;

        std::string button_name = (is_expanded) ? std::string(ICON_EXPAND_OPEN) + "##BUTTON_" + pri_name : std::string(ICON_EXPAND_CLOSE) + "##BUTTON_" + pri_name;

        ImGui::PushStyleColor(ImGuiCol_Button, button_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color);
        
        ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_SMALL));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);
        ImGui::Button(button_name.c_str());
        ImGui::PopFont();
        
        if(ImGui::IsItemClicked()){
            if(is_expanded){
                ObjectServer::Ref()->get_instance<GraphBase>(pri_id)->collapse_on_list();
            }else{
                ObjectServer::Ref()->get_instance<GraphBase>(pri_id)->expand_on_list();
            }
        }
        //ImGui::PopFont();
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetItemRectSize().y / 2.0f);

        ImGui::Selectable((pri_name).c_str(), is_selected, 0, inpector_item_size);
        if(ImGui::IsItemHovered()){
            MsgGenerator gen;
            gen.add_row("Type", ObjectServer::Ref()->get_instance<GraphBase>(pri_id)->get_type_name());

            QuickTextDisplay::Ref()->set_text(gen.get());
            QuickTextDisplay::Ref()->set_font_size(18.0f);
            QuickTextDisplay::Ref()->show();
        }

        m_update_item_status(pri_id);

        if (is_selected){
            ImVec2 p_min = ImGui::GetItemRectMin();
            ImVec2 p_max = {ImGui::GetItemRectMin().x + m_hint_width, ImGui::GetItemRectMax().y};

            ImGui::GetWindowDrawList()->AddRectFilled(
                p_min, p_max,
                sub_area_color,
                0.0f,
                0
            );
        }
        ImVec2 break_line_min = {ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y - 1.0f};
        ImVec2 break_line_max = {ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y};
        ImGui::GetWindowDrawList()->AddRectFilled(
            break_line_min, break_line_max,
            break_color,
            0.5f,
            0
        );


        ImGui::PopStyleColor(5);
        
        m_iterated_ids.push_back(pri_id);
        m_index++;

        m_update_inpector_secondary_list(p);
    }
}

void EditorLeftPanel::m_update_inpector_secondary_list(int p_parent_index){
    auto& pri_item_list = GraphManager::Ref()->get_panel_data().primary_info_list;
    auto& sec_item_list = GraphManager::Ref()->get_panel_data().secondary_info_list[p_parent_index];
    const int sec_item_size = sec_item_list.size();

    auto& parent_id = pri_item_list[p_parent_index];

    if (sec_item_size > 0 && GraphManager::Ref()->get_node_info(parent_id).is_expanded){
        const std::string sec_list_box_name = "##InspectorSecListBox";

        const double sec_item_height = inpector_item_size.y + 3;
        const double sec_list_area_height = (sec_item_size == 0) ? 0.0f : sec_item_height * sec_item_size;
        const ImVec2 sec_list_area = ImVec2(-FLT_MIN, sec_list_area_height);
                          
        ImVec4 sec_list_box_color_org = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");
        const ImVec4 sec_list_box_color = {sec_list_box_color_org.x - 0.02f, sec_list_box_color_org.y - 0.02f, sec_list_box_color_org.z - 0.02f, sec_list_box_color_org.w};
        const ImVec4 sec_list_box_selected_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
        const ImVec4 sec_list_box_text_color_selected = ThemeLoader::Ref()->get_imgui_color("HighlightTextColour");
        const ImVec4 sec_list_box_text_color_not_selected = ThemeLoader::Ref()->get_imgui_color("TextColour");
        const ImU32 sub_area_color = ThemeLoader::Ref()->get_imgui_color_int("AccentColour1");
        const ImU32 breakline_color = ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour3");

        if (ImGui::BeginListBox((sec_list_box_name + std::to_string(p_parent_index)).c_str(), sec_list_area)){
            for (int s = 0; s < sec_item_size; ++s){
                auto sec_id = sec_item_list[s];
                const GraphManager::NodeInfo& node_info = GraphManager::Ref()->get_node_info(sec_id);

                const bool is_selected = (GraphSelection::Ref()->is_id_in_dragging_buffer(sec_id)) ? true :  node_info.is_selected;

                auto sec_type = node_info.type;
                std::string padding = (is_selected) ? "    " : "";
                auto sec_name = padding + int_to_roman(s + 1) + " - " + node_info.name;

                const ImVec4& sec_list_box_text_color = (is_selected) ? sec_list_box_text_color_selected : sec_list_box_text_color_not_selected;

                ImGui::PushStyleColor(ImGuiCol_FrameBg, sec_list_box_color);
                ImGui::PushStyleColor(ImGuiCol_Header, sec_list_box_selected_color);
                ImGui::PushStyleColor(ImGuiCol_Text, sec_list_box_text_color);


                ImGui::Selectable(("  "+sec_name).c_str(), is_selected,0 ,inpector_item_size);
                if(ImGui::IsItemHovered()){
                    MsgGenerator gen;
                    gen.add_row("Type", ObjectServer::Ref()->get_instance<GraphBase>(sec_id)->get_type_name());

                    QuickTextDisplay::Ref()->set_text(gen.get());
                    QuickTextDisplay::Ref()->set_font_size(18.0f);
                    QuickTextDisplay::Ref()->show();
                }

                m_update_item_status(sec_id);


                if (is_selected){
                    ImVec2 p_min = ImGui::GetItemRectMin();
                    ImVec2 p_max = {ImGui::GetItemRectMin().x + m_hint_width, ImGui::GetItemRectMax().y};

                    ImGui::GetWindowDrawList()->AddRectFilled(
                        p_min, p_max,
                        sub_area_color,
                        0.0f,
                        0
                    );
                }
                ImVec2 break_line_min = {ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y - 1.0f};
                ImVec2 break_line_max = {ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y};
                ImGui::GetWindowDrawList()->AddRectFilled(
                    break_line_min, break_line_max,
                    breakline_color,
                    0.5f,
                    0
                );


                ImGui::PopStyleColor(3);

                m_iterated_ids.push_back(sec_id);
                m_index++;
            }
            ImGui::EndListBox();
        }
    }
}

void EditorLeftPanel::m_update_inpector_other_list(){
    auto& other_item_list = GraphManager::Ref()->get_panel_data().other_info_list;

    if(!other_item_list.empty()){
        ImGui::Separator();
    }

    const int other_item_size = other_item_list.size();
    if (other_item_size > 0){
        const char* other_list_box_name = "##InspectorOtherListBox";
        const double other_item_height = inpector_item_size.y + 3.0f;
        const double other_list_area_height = (other_item_size == 0) ? 0.0f : other_item_height * other_item_size;
        const ImVec2 other_list_area = ImVec2(-FLT_MIN, other_list_area_height);

        if (ImGui::BeginListBox(other_list_box_name, other_list_area)){
            const ImVec4 other_list_box_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
            const ImVec4 other_list_box_selected_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
            const ImVec4 other_list_box_text_color_selected = ThemeLoader::Ref()->get_imgui_color("HighlightTextColour");
            const ImVec4 other_list_box_text_color_not_selected = ThemeLoader::Ref()->get_imgui_color("TextColour");
            const ImU32 sub_area_color = ThemeLoader::Ref()->get_imgui_color_int("AccentColour1");
            const ImU32 breakline_color = ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour3");

            for (int o = 0; o < other_item_size; ++o){
                auto other_id = other_item_list[o];
                const GraphManager::NodeInfo& node_info = GraphManager::Ref()->get_node_info(other_id);

                const bool is_selected = (GraphSelection::Ref()->is_id_in_dragging_buffer(other_id)) ? true :  node_info.is_selected;

                std::string padding = (is_selected) ? "    " : "";
                auto other_name = padding + "*" + std::to_string(o + 1) + " - " + node_info.name;

                const ImVec4 other_list_box_text_color = (is_selected) ? other_list_box_text_color_selected : other_list_box_text_color_not_selected;

                ImGui::PushStyleColor(ImGuiCol_FrameBg, other_list_box_color);
                ImGui::PushStyleColor(ImGuiCol_Header, other_list_box_selected_color);
                ImGui::PushStyleColor(ImGuiCol_Text, other_list_box_text_color);

                ImGui::Selectable((other_name).c_str(), is_selected, 0, inpector_item_size);
                if(ImGui::IsItemHovered()){
                    MsgGenerator gen;
                    gen.add_row("Type", ObjectServer::Ref()->get_instance<GraphBase>(other_id)->get_type_name());

                    QuickTextDisplay::Ref()->set_text(gen.get());
                    QuickTextDisplay::Ref()->set_font_size(18.0f);
                    QuickTextDisplay::Ref()->show();
                }
                

                m_update_item_status(other_id);

                if (is_selected){
                    ImVec2 p_min = ImGui::GetItemRectMin();
                    ImVec2 p_max = {ImGui::GetItemRectMin().x + m_hint_width, ImGui::GetItemRectMax().y};
                    ImGui::GetWindowDrawList()->AddRectFilled(
                        p_min, p_max,
                        sub_area_color,
                        0.0f,
                        0
                    );
                }
                ImVec2 break_line_min = {ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y - 1.0f};
                ImVec2 break_line_max = {ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y};
                ImGui::GetWindowDrawList()->AddRectFilled(
                    break_line_min, break_line_max,
                    breakline_color,
                    0.5f,
                    0
                );


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