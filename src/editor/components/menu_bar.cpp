#include "menu_bar.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_video.h"
#include "engine/window.h"
#include "imgui/imgui.h"

EditorMenuBar::EditorMenuBar(){
    BIND_CLASS(EditorMenuBar);
    m_double_click_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::MILLISECOND, 200) ,true);
    m_double_click_timer->stop();
}
EditorMenuBar::~EditorMenuBar(){

}

void EditorMenuBar::ready(){
    this->disable_cursor_change();
}
void EditorMenuBar::pre_process(){
    m_update_shape();
    m_reset_vars();
    m_begin_main_bar();
    m_update_menu_file();
    m_update_menu_edit();
    m_update_maximize_button();
    m_update_minimize_button();
    m_update_close_button();
    m_end_main_bar();
    m_handle_window();
    m_block_resizer();
}
void EditorMenuBar::process(){
}
void EditorMenuBar::post_process(){

}
void EditorMenuBar::draw(){

}


void EditorMenuBar::ui_init(EditorSpace* p_space){
    m_space = p_space;
}

void EditorMenuBar::m_update_shape(){
    m_shape = m_space->to_world();
    this->set_shape(m_shape);
}
void EditorMenuBar::m_reset_vars(){
    is_hover_any = false;
}
void EditorMenuBar::m_begin_main_bar(){
    double bar_size = m_space->get_size().y;

    float font_size = ImGui::GetFontSize();
    float border = ImGui::GetStyle().FrameBorderSize;
    float target_padding_y = (bar_size - font_size - border * 2.0f) / 2.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, target_padding_y));
    ImGui::BeginMainMenuBar();
}
void EditorMenuBar::m_update_menu_file(){
    if (ImGui::BeginMenu("File")){
        ImGui::MenuItem("New");
        ImGui::MenuItem("Open");
        ImGui::EndMenu();
    }
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
}
void EditorMenuBar::m_update_menu_edit(){
    if (ImGui::BeginMenu("Edit")){
        ImGui::MenuItem("Cut");
        ImGui::MenuItem("Copy");
        ImGui::EndMenu();
    }
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
}
void EditorMenuBar::m_update_minimize_button(){
    std::string name = "-";

    ImGui::PushFont(EngineFontLoader::Ref()->get(EngineFontLoader::UI_ICON_MIDDLE));
    float text_height = ImGui::CalcTextSize(name.c_str()).y;
    float full_height = ImGui::GetContentRegionAvail().y;
    float padding_y = std::max(0.0f, (full_height - text_height) + 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f,0.5f));

    const int button_count = 3;

    float button_width = 45.0f;
    float spacing = 0;
    float text_width = ImGui::CalcTextSize(name.c_str()).x;
    float window_width = ImGui::GetWindowWidth();
    float total_button_width = (button_width * button_count) + spacing * (button_count - 1);
    float start_x = window_width - total_button_width - 5.0f;
    float padding_x = std::max(0.0f, (button_width - text_width) * 0.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding_x, padding_y));
    ImGui::SameLine(start_x);
    
    ImGui::Button(name.c_str());
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) {
        EngineWindow::Ref()->minimize();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopFont();
}
void EditorMenuBar::m_update_maximize_button(){
    std::string name = "+";

    if(EngineWindow::Ref()->is_maximized()){
        name = "=";
    }

    ImGui::PushFont(EngineFontLoader::Ref()->get(EngineFontLoader::UI_ICON_MIDDLE));
    float text_height = ImGui::CalcTextSize(name.c_str()).y;
    float full_height = ImGui::GetContentRegionAvail().y;
    float padding_y = std::max(0.0f, (full_height - text_height) + 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f,0.5f));


    const int button_count = 2;

    float button_width = 45.0f;
    float spacing = 0;
    float text_width = ImGui::CalcTextSize(name.c_str()).x;
    float window_width = ImGui::GetWindowWidth();
    float total_button_width = (button_width * button_count) + spacing * (button_count - 1);
    float start_x = window_width - total_button_width - 5.0f;
    float padding_x = std::max(0.0f, (button_width - text_width) * 0.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding_x, padding_y));
    ImGui::SameLine(start_x);

    ImGui::Button(name.c_str());
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) {
        if(EngineWindow::Ref()->is_maximized()){
            EngineWindow::Ref()->restore();
        }else{
            EngineWindow::Ref()->maximize();
        }
    }
    ImGui::PopStyleVar(2);
    ImGui::PopFont();
}
void EditorMenuBar::m_update_close_button(){
    std::string name = "x";

    ImGui::PushFont(EngineFontLoader::Ref()->get(EngineFontLoader::UI_ICON_MIDDLE));
    float text_height = ImGui::CalcTextSize(name.c_str()).y;
    float full_height = ImGui::GetContentRegionAvail().y;
    float padding_y = std::max(0.0f, (full_height - text_height) + 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f,0.5f));

    const int button_count = 1;

    float button_width = 45.0f;
    float spacing = 0;
    float text_width = ImGui::CalcTextSize(name.c_str()).x;
    float window_width = ImGui::GetWindowWidth();
    float total_button_width = (button_width * button_count) + spacing * (button_count - 1);
    float start_x = window_width - total_button_width - 5.0f;
    float padding_x = std::max(0.0f, (button_width - text_width) * 0.5f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding_x, padding_y));
    ImGui::SameLine(start_x);
    
    ImGui::Button(name.c_str());
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) {
        EngineWindow::Ref()->close();
    }
    ImGui::PopStyleVar(2);
    ImGui::PopFont();
}
void EditorMenuBar::m_end_main_bar(){
    ImGui::EndMainMenuBar();
    ImGui::PopStyleVar();
}
void EditorMenuBar::m_handle_window(){
    if(!m_double_click_timer->is_timeout() && !is_hover_any && this->was_just_clicked()){
        EngineWindow::Ref()->stop_dragging();
        if(EngineWindow::Ref()->is_maximized()){
            EngineWindow::Ref()->restore();
        }else{
            EngineWindow::Ref()->maximize();
        }

        return;
    }

    if(!is_hover_any && this->was_just_clicked()){
        m_double_click_timer->reset();
        m_double_click_timer->start();
        EngineWindow::Ref()->start_dragging();
    }

    if(this->was_just_released()){
        EngineWindow::Ref()->stop_dragging();
    }

    if(m_double_click_timer->is_timeout()){
        m_double_click_timer->stop();
    }
}

void EditorMenuBar::m_block_resizer(){
    if(is_hover_any){
        EventServer::Ref()->block<EventMouseOnResizer>();
    }
}