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
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <new>
#include <server/file_server.h>
#include <string>
#include <system_error>
#include <theme/theme_loader.h>
#include <vector>

#include "ext/debug/messenger_ext.h"
#include "server/ui_icon_unicode.h"

ExplorerWindow::ExplorerWindow(){
    BIND_CLASS(ExplorerWindow);
}
ExplorerWindow::~ExplorerWindow(){

}

void ExplorerWindow::ready(){
}

void ExplorerWindow::Page::last_page(){
    from -= size;
    to -= size;

    DEBUG_MSG("Page :" << from << "-" << to);
}
void ExplorerWindow::Page::next_page(){
    from = to + 1;
    to += size;

    DEBUG_MSG("Page :" << from << "-" << to);
}
void ExplorerWindow::Page::reset(){
    from = 1;
    to = size;
    
    DEBUG_MSG("Page :" << from << "-" << to);
}
bool ExplorerWindow::Page::is_in_page(int p_index){
    return from <= p_index && p_index <= to;
}
bool ExplorerWindow::Page::is_at_first_page(){
    return from <= min;
}
bool ExplorerWindow::Page::is_at_last_page(){
    return to > max;
}

void ExplorerWindow::m_go_to_dir(std::filesystem::path p_path){
    std::string str_path = p_path.string();
    ExplorerWindowDataPipeline::Ref()->dir_path = str_path;
    m_selected_path = str_path;

    page.reset();

    m_is_refresh_needed = true;
}
void ExplorerWindow::m_close_button_process(){
    std::string bk_button_id = "<";
    bk_button_id += "##";
    bk_button_id += m_uid;

    float content_width = ImGui::GetContentRegionAvail().x + 10;
    float button_width = 20.0f;
    float button_height = 20.0f;
    float button_x = content_width - button_width;

    ImGui::PushFont(EngineFontLoader::Ref()->get(12));
    if(ImGui::Button(bk_button_id.c_str(), ImVec2(button_width, button_height))){
        std::filesystem::path p(ExplorerWindowDataPipeline::Ref()->dir_path);
        p = std::filesystem::absolute(p);
        if(p.has_parent_path()){
            m_go_to_dir(p.parent_path());
        }
    }
    
    std::string refresh_button_id = ICON_REFRESH;
    refresh_button_id += "##";
    refresh_button_id += m_uid;

    ImGui::SameLine(0.0f);
    if(ImGui::Button(refresh_button_id.c_str(), ImVec2(button_width, button_height))){
        m_is_refresh_needed = true;
    }
    ImGui::PopFont();

    ImGui::SameLine(0.0f);
    ImVec2 text_cursor = ImGui::GetCursorPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    vec2 min = m_win_pos + vec2(text_cursor.x, 10.0f - 2.0f);
    vec2 max = m_win_pos + vec2(15.0f + m_win_size.x - 55.0f, 25.0f + 4.0f);
    drawList->AddRectFilled({min.x, min.y}, {max.x, max.y}, ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour3"));

    ImGui::SetCursorPos({text_cursor.x, text_cursor.y - 2.0f});
    ImGui::Text("  %s", m_selected_path.c_str());

    ImGui::SameLine(button_x);

    std::string button_id = ICON_WIN_CLOSE;
    button_id += "##";
    button_id += m_uid;

    ImGui::PushFont(EngineFontLoader::Ref()->get(12));
    if(ImGui::Button(button_id.c_str(), ImVec2(button_width, button_height))){
        this->queue_free();
    }
    ImGui::PopFont();
    if(ImGui::IsItemHovered()){
        EventServer::Ref()->emit_locked_all();
    }
}
void ExplorerWindow::m_generate_display_list(){
    auto dir_path = std::filesystem::absolute(ExplorerWindowDataPipeline::Ref()->dir_path);
    auto dir_it = std::filesystem::directory_iterator(dir_path, std::filesystem::directory_options::skip_permission_denied);
    std::vector<FObjectRow> rows;

    int index = 0;
    for(auto& base_entry : dir_it){
        index++;
        FObjectRow row;
        row.abs_path = std::filesystem::absolute(base_entry.path());
        row.str_path = row.abs_path.string();
        row.name = row.abs_path.filename().string();
        row.display_name = row.name + "##" + std::to_string(index);
        row.is_hidden = FileServer::Ref()->is_file_hidden(row.abs_path);
        row.is_directory = base_entry.is_directory();
        row.index = (row.is_directory) ? index - 100000 : index;
        if(row.is_directory){
            row.is_opened = ExplorerWindowDataPipeline::Ref()->opened_folder.contains(row.str_path);

            if(row.is_opened){
                row.display_name = ICON_FOLDER_OPEN + row.name;
            }else{
                row.display_name = ICON_FOLDER + row.name;
            }

            auto sub_dir_it = std::filesystem::directory_iterator(row.abs_path, std::filesystem::directory_options::skip_permission_denied);
            for(auto& sub_entry : sub_dir_it){
                index++;
                FObjectRow sub_row;
                sub_row.abs_path = std::filesystem::absolute(sub_entry.path());
                sub_row.str_path = sub_row.abs_path.string();
                sub_row.name = sub_row.abs_path.filename().string();
                sub_row.display_name = "      " + sub_row.name + "##" + std::to_string(index);
                sub_row.is_hidden = FileServer::Ref()->is_file_hidden(sub_row.abs_path);
                sub_row.is_directory = sub_entry.is_directory();
                sub_row.index = (sub_row.is_directory) ? index - 100000 : index;
                row.children.push_back(sub_row);
            }
        }
        rows.push_back(row);
    }
    m_display_list.swap(rows);

    std::sort(m_display_list.begin(), m_display_list.end(), [](FObjectRow a, FObjectRow b){
        return a.index < b.index;
    });

    index = 1;
    page.min = index;
    page.max = index;
    for(FObjectRow& row : m_display_list){
        row.index = index;

        const int children_max = 5;
        bool is_children_overflow = false;
        int sub_index = 0;

        std::vector<FObjectRow> children_buf;
        for(FObjectRow& sub_row : row.children){
            sub_row.index = index;
            if(sub_index < children_max){
                children_buf.push_back(sub_row);
            }else{
                is_children_overflow = true;
            }

            sub_index++;
        }

        if(is_children_overflow){
            row.children.swap(children_buf);
            FObjectRow show_more_row = row;
            show_more_row.name = "...";
            show_more_row.display_name = "...";
            show_more_row.children.clear();
            row.children.push_back(show_more_row);
        }

        page.max = index;
        index++;
    }
}
void ExplorerWindow::m_update_render_data(){
    m_engine_win_size = EngineWindow::Ref()->get_window_size();
    m_win_size = {m_engine_win_size.x * 0.75f, m_engine_win_size.y * 0.65f};
    m_win_pos = {m_engine_win_size.x / 2.0f - m_win_size.x / 2.0f,
                                m_engine_win_size.y / 2.0f - m_win_size.y / 2.0f};

    const float padding_x = 15.0f;
    const float padding_y = 10.0f;
    const float non_explorer_area_height = 25.0f;
    const float close_button_offset = 25.0f;

    m_window_size = {m_win_size.x ,m_win_size.y};
    m_explorer_area_size = {m_window_size.x - padding_x * 2.0f, m_window_size.y - padding_y * 2.0f - non_explorer_area_height};
    m_explorer_area_pos = {padding_x, padding_y + close_button_offset};
}
void ExplorerWindow::m_update_opened_folder(){
    for(FObjectRow& row : m_display_list){
        row.is_opened = ExplorerWindowDataPipeline::Ref()->opened_folder.contains(row.str_path);
        if(row.is_directory){
            row.display_name = " " + row.name + "##" + std::to_string(row.index);;
            if(row.is_opened){
                row.display_name = ICON_FOLDER_OPEN + row.display_name;
            }else{
                row.display_name = ICON_FOLDER + row.display_name;
            }
            for(FObjectRow& sub_row : row.children){
                sub_row.is_opened = ExplorerWindowDataPipeline::Ref()->opened_folder.contains(sub_row.str_path);
                sub_row.display_name = "      " + sub_row.name + "##" + std::to_string(sub_row.index);
            }
        }
    }
}
void ExplorerWindow::m_explorer_area_process_draw_area_background(){
    m_draw_list->AddRectFilled({m_explorer_area_pos.x + m_win_pos.x, m_explorer_area_pos.y + m_win_pos.y},
        {m_explorer_area_pos.x + m_explorer_area_size.x + m_win_pos.x, m_explorer_area_pos.y + m_explorer_area_size.y + m_win_pos.y},
        ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour3"));
}
void ExplorerWindow::m_explorer_area_process_create_main_selectable(){
    ImGui::Selectable(m_main_row.display_name.c_str());
    if(ImGui::IsItemHovered()){
        EventServer::Ref()->emit_locked_all();
        if(MouseServer::Ref()->is_mouse_multi_clicked(1)){
            if(m_main_row.is_opened){
                ExplorerWindowDataPipeline::Ref()->opened_folder.erase(m_main_row.str_path);
            }else{
                ExplorerWindowDataPipeline::Ref()->opened_folder.emplace(m_main_row.str_path);
            }
            m_selected_path = m_main_row.str_path;
        }
        if(MouseServer::Ref()->is_mouse_multi_clicked(2)){
            if(m_main_row.is_directory && FileServer::Ref()->is_file_exists(m_main_row.abs_path)){
                m_go_to_dir(m_main_row.abs_path);
            }
        }
    }
}
void ExplorerWindow::m_explorer_area_process_create_sub_selectable(){
    ImGui::Selectable(m_sub_row.display_name.c_str());
    if(ImGui::IsItemHovered()){
        EventServer::Ref()->emit_locked_all();
        if(MouseServer::Ref()->is_mouse_multi_clicked(1)){
            m_selected_path = m_sub_row.str_path;
        }
        if(MouseServer::Ref()->is_mouse_multi_clicked(2)){
            if(m_sub_row.is_directory && FileServer::Ref()->is_file_exists(m_sub_row.abs_path)){
                m_go_to_dir(m_sub_row.abs_path);
            }
        }
    }
}
void ExplorerWindow::m_explorer_area_process_draw_split_line(){
    vec2 item_split_start = vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y);
    vec2 item_split_end = vec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
    m_draw_list->AddLine({item_split_start.x, item_split_start.y}, {item_split_end.x, item_split_end.y}, ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour2"));
}
void ExplorerWindow::m_explorer_area_process_draw_folder_line(){
    vec2 line_start = vec2(ImGui::GetItemRectMin().x + ImGui::CalcTextSize("      ").x / 2.0f, ImGui::GetItemRectMin().y);
    vec2 line_end = line_start + vec2(0.0f, ImGui::GetItemRectSize().y);
    m_draw_list->AddLine({line_start.x, line_start.y}, {line_end.x, line_end.y}, ThemeLoader::Ref()->get_imgui_color_int("TextColour"));
    vec2 line2_start = vec2(line_start.x, line_start.y + ImGui::GetItemRectSize().y);
    vec2 line2_end = vec2(line2_start.x + ImGui::CalcTextSize("      ").x / 2.0f, line2_start.y);
    m_draw_list->AddLine({line2_start.x, line2_start.y}, {line2_end.x, line2_end.y}, ThemeLoader::Ref()->get_imgui_color_int("TextColour"));
    vec2 item_split_start = vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y);
    vec2 item_split_end = vec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
    m_draw_list->AddLine({item_split_start.x, item_split_start.y}, {item_split_end.x, item_split_end.y}, ThemeLoader::Ref()->get_imgui_color_int("SecondaryColour2"));
}
void ExplorerWindow::m_explorer_area_process(){
    std::string uid = "##ExplorerListBox";
    uid += m_uid; 

    ImGui::SetCursorPos({m_explorer_area_pos.x, m_explorer_area_pos.y});
    if (ImGui::BeginListBox(uid.c_str(), ImVec2(m_explorer_area_size.x, m_explorer_area_size.y))){
        m_draw_list = ImGui::GetWindowDrawList();
        m_explorer_area_process_draw_area_background();

        if(!page.is_at_first_page()){
            std::string last_page_uid = ICON_LAST_PAGE;
            last_page_uid += "##" + uid + "_LAST_PAGE";
            ImGui::Selectable(last_page_uid.c_str());
            if(ImGui::IsItemHovered()){
                EventServer::Ref()->emit_locked_all();
                if (MouseServer::Ref()->is_just_released()){
                    page.last_page();
                }
            }
            m_explorer_area_process_draw_split_line();
        }

        for(auto& main_row : m_display_list){
            m_main_row = main_row;

            if(m_main_row.is_hidden){
                continue;
            }

            if(!page.is_in_page(m_main_row.index)){
                continue;
            }

            m_explorer_area_process_create_main_selectable();
            m_explorer_area_process_draw_split_line();

            if(m_main_row.is_directory){
                if(m_main_row.is_opened){
                    for(auto& sub_row : m_main_row.children){
                        m_sub_row = sub_row;

                        if(m_sub_row.is_hidden){
                            continue;
                        }

                        if(!page.is_in_page(m_sub_row.index)){
                            continue;
                        }

                        m_explorer_area_process_create_sub_selectable();
                        m_explorer_area_process_draw_folder_line();
                    }
                }
            }
        }
       
        if(!page.is_at_last_page()){
            std::string nest_page_uid = ICON_NEXT_PAGE;
            nest_page_uid += "##" + uid + "_NEXT_PAGE";
            ImGui::Selectable(nest_page_uid.c_str());
            if(ImGui::IsItemHovered()){
                EventServer::Ref()->emit_locked_all();
                if (MouseServer::Ref()->is_just_released()){
                    page.next_page();
                }
            }
            m_explorer_area_process_draw_split_line();
        }

        ImGui::EndListBox();
    }
    
}
    
void ExplorerWindow::pre_process(){
    if(m_is_refresh_needed){
        m_generate_display_list();
        m_is_refresh_needed = false;
    }

    m_update_opened_folder();
    m_update_render_data();

    ImGui::SetNextWindowPos({m_win_pos.x, m_win_pos.y});
    ImGui::SetNextWindowSize({m_win_size.x, m_win_size.y});
    ImGui::SetNextWindowFocus();
    ImGui::Begin("##EXPLORER_WINDOW", NULL, ImGuiWindowFlags_NoDecoration);
        m_close_button_process();
        m_explorer_area_process();
        if(ImGui::IsWindowFocused()){
            EventServer::Ref()->emit_locked_all();
        }
    ImGui::End();
}
void ExplorerWindow::process(){

}
void ExplorerWindow::post_process(){

}
void ExplorerWindow::draw(){

}
