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

        ImGui::Begin("EditorLeftPanel", &is_display, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
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