#include "menu_bar.h"
#include "config/config_loader.h"
#include "server/project_server.h"
#include "server/ui_icon_unicode.h"
#include "server/ui_text_bank.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_video.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"

EditorMenuBar::EditorMenuBar(){
    BIND_CLASS(EditorMenuBar);
    m_double_click_timer = TimerServer::Ref()->create_timer(TimeUnit(TimeUnit::Type::MILLISECOND, 200) ,true);
    m_double_click_timer->stop();
}
EditorMenuBar::~EditorMenuBar(){

}

void EditorMenuBar::ready(){
    this->disable_cursor_change();
    this->set_hovering_type(Type::SCREEN);
}
void EditorMenuBar::pre_process(){
    m_update_shape();
    m_reset_vars();
    m_begin_main_bar();
    m_update_menu_file();
    //m_update_menu_edit();
    m_update_menu_themes();
    m_update_menu_language();
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
    m_shape = *(Rect2*)m_space;
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

    //ImGui::SetNextWindowFocus();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, target_padding_y));
    ImGui::BeginMainMenuBar();
}
void EditorMenuBar::m_update_menu_file(){
    if (ImGui::BeginMenu(UiTextBank::Ref()->File)){
        ImGui::MenuItem(UiTextBank::Ref()->New);
        if(ImGui::MenuItem(UiTextBank::Ref()->SaveAllWS)){
            ProjectServer::Ref()->save_all_workspaces();
        }
        if(ImGui::MenuItem(UiTextBank::Ref()->SaveAsPJ)){
            ProjectServer::Ref()->save_as_project("E:\\test.dlg");
        }
        ImGui::MenuItem(UiTextBank::Ref()->Open);
        ImGui::EndMenu();
    }
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
}
void EditorMenuBar::m_update_menu_language(){
    if (ImGui::BeginMenu(UiTextBank::Ref()->Language)){
        for(auto locale : UiTextBank::Ref()->all_locales){
            ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_MIDDLE));

            ImGui::MenuItem(locale->get_name());
            if(ImGui::IsItemClicked()){
                UiTextBank::Ref()->set_locale(locale->get_locale_id());
            }

            ImGui::PopFont();
        }
        ImGui::EndMenu();
    }
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
}
void EditorMenuBar::m_update_menu_edit(){
    if (ImGui::BeginMenu(UiTextBank::Ref()->Edit)){
        ImGui::MenuItem(UiTextBank::Ref()->Cut);
        ImGui::MenuItem(UiTextBank::Ref()->Copy);
        ImGui::EndMenu();
    }
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
}
void EditorMenuBar::m_update_menu_themes(){
    std::string current_theme;
    ConfigLoader::Ref()->get_config("UsingTheme", current_theme);

    if (ImGui::BeginMenu(UiTextBank::Ref()->Themes)){
        auto themes = ThemeLoader::Ref()->get_themes();
        for(auto theme : themes){
            if(current_theme == theme){
                vec4 text_colour = ThemeLoader::Ref()->get_color("HighlightTextColour");
                ImVec4 text_colour_imgui = ImVec4(text_colour.x, text_colour.y, text_colour.z, text_colour.w);
                vec4 item_colour = ThemeLoader::Ref()->get_color("AccentColour2");
                ImVec4 item_colour_imgui = ImVec4(item_colour.x, item_colour.y, item_colour.z, item_colour.w);

                ImGui::PushStyleColor(ImGuiCol_Text, text_colour_imgui);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, item_colour_imgui);
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, item_colour_imgui);
            }
            ImGui::MenuItem(theme.c_str());
            if(current_theme == theme){
                ImGui::PopStyleColor(3);
            }


            if(ImGui::IsItemClicked()){
                ConfigLoader::Ref()->set_config("UsingTheme", theme);
                EditorLayout::Ref()->refresh_theme();
                ConfigLoader::Ref()->save();
            }
        }
        ImGui::EndMenu();
    }
    if (ImGui::IsItemHovered()) {
        is_hover_any = true;
    }
}
void EditorMenuBar::m_update_minimize_button(){
    std::string name = ICON_WIN_MINIMIZE;

    ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_MIDDLE));
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
    std::string name = ICON_WIN_MAXIMIZE;

    if(EngineWindow::Ref()->is_maximized()){
        name = ICON_WIN_RESTORE;
    }

    ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_MIDDLE));
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
    std::string name = ICON_WIN_CLOSE;

    ImGui::PushFont(EngineFontLoader::Ref()->get(FONT_SIZE_MIDDLE));
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


    vec4 close_button_colour = ThemeLoader::Ref()->get_color("CloseButtonColour");
    ImVec4 clouse_button_colour_imgui = ImVec4(close_button_colour.x, close_button_colour.y, close_button_colour.z, close_button_colour.w);

    ImGui::PushStyleColor(ImGuiCol_ButtonActive, clouse_button_colour_imgui);
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
    ImGui::PopStyleColor();
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