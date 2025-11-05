#include "menu_bar.h"
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
    this->set_shape(*m_space);
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
    const char* name = "-";

    float button_width = 30.0f;
    int button_count = 3;
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    float window_width = ImGui::GetWindowWidth();

    float total_button_width = (button_width * button_count) + spacing * (button_count - 1) + button_width * 0.5;

    float x = window_width - total_button_width;

    ImGui::SameLine(x);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(button_width / 2.0f, button_width / 2.0f));
    
    ImGui::Button(name);
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
    if (ImGui::IsItemClicked()) {
        EngineWindow::Ref()->minimize();
    }
    ImGui::PopStyleVar();
}
void EditorMenuBar::m_update_maximize_button(){
    const char* name = "+";

    float button_width = 30.0f;
    int button_count = 2;
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    float window_width = ImGui::GetWindowWidth();

    float total_button_width = (button_width * button_count) + spacing * (button_count - 1) + button_width * 0.5;

    float x = window_width - total_button_width;

    ImGui::SameLine(x);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(button_width / 2.0f, button_width / 2.0f));
    
    ImGui::Button(name);
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
    if (ImGui::IsItemClicked()) {
        if(EngineWindow::Ref()->is_maximized()){
            EngineWindow::Ref()->restore();
        }else{
            EngineWindow::Ref()->maximize();
        }
    }
    ImGui::PopStyleVar();
}
void EditorMenuBar::m_update_close_button(){
    const char* name = "X";

    float button_width = 30.0f;
    int button_count = 1;
    float spacing = ImGui::GetStyle().ItemSpacing.x;

    float window_width = ImGui::GetWindowWidth();

    float total_button_width = (button_width * button_count) + spacing * (button_count - 1) + button_width * 0.5;

    float x = window_width - total_button_width;

    ImGui::SameLine(x);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(button_width / 2.0f, button_width / 2.0f));
    
    ImGui::Button(name);
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
    if (ImGui::IsItemClicked()) {
        EngineWindow::Ref()->close();
    }
    ImGui::PopStyleVar();
}
void EditorMenuBar::m_end_main_bar(){
    ImGui::EndMainMenuBar();
    ImGui::PopStyleVar();
}
void EditorMenuBar::m_handle_window(){
    if(!m_double_click_timer->is_timeout() && !is_hover_any && this->was_just_clicked()){
        DEBUG_MSG("DOUBLE CLICKED");
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

