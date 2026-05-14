#include "editor/components/tools_bar.h"
#include "server/ui_text_bank.h"
#include "editor/components/messager.h"
#include "editor/components/popup_window.h"
#include "editor/layout.h"
#include "engine/font_loader.h"
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
#include "system/graph/camera.h"
#include "system/graph/grid.h"
#include "system/obj/graph/manager.h"
#include <sstream>
#include <string>

EditorToolsBar::EditorToolsBar(){
    BIND_CLASS(EditorToolsBar);
    m_double_click_timer.stop();
}
EditorToolsBar::~EditorToolsBar(){

}

void EditorToolsBar::ready(){
    this->disable_cursor_change();
    this->set_hovering_type(Type::SCREEN);

}
void EditorToolsBar::pre_process(){
    m_update_shape();

    Rect2 window_rect = EditorLayout::Ref()->covnert_to_window(*(Rect2*)m_space);
    ImGui::SetNextWindowSize({window_rect.get_size().x, window_rect.get_size().y});
    ImGui::SetNextWindowPos({window_rect.get_left_top().x, window_rect.get_left_top().y});

    ImGui::Begin("EditorToolsBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if(ImGui::Button(UiTextBank::Ref()->BackToCenter)){
        GraphCamera::Ref()->set_zoom(1.0f);
        vec2 center_pos = vec2(0.0f,0.0f) + (GraphCamera::Ref()->get_zoomed_size() / 2.0f);
        GraphCamera::Ref()->set_target(center_pos);
    }

    ImGui::SameLine();

    ImGui::Button(UiTextBank::Ref()->GoTo);
    if(ImGui::IsItemClicked()){
        if(PopupWindowManager::Ref()->is_window_exists(m_go_to_window.uid)){
            m_go_to_window.ptr->close();
        }

        PopupWindow* window = ObjectServer::Ref()->queue_create<PopupWindow>();
        
        m_go_to_window.ptr = window;
        m_go_to_window.uid = window->get_uid();

        window->set_content(UiTextBank::Ref()->GoToContent);

        window->add_input("X :", PopupWindow::InputType::STRING);
        window->add_input("Y :", PopupWindow::InputType::STRING);

        window->add_option(UiTextBank::Ref()->Go, [window](){
            std::string x = window->get_input_string(0);
            std::string y = window->get_input_string(1);
            if(GraphGrid::Ref()->is_id_exists(x, y)){
                GraphCamera::Ref()->set_zoom(1.0f);
                vec2 go_to_pos = GraphGrid::Ref()->get_pos(x, y) + (GraphCamera::Ref()->get_zoomed_size() / 2.0f);
                GraphCamera::Ref()->set_target(go_to_pos);
                window->close();
            }else{
                std::string msg = UiTextBank::Ref()->GoToNotExistsError;
                if(x.size() > 0 && y.size() > 0){
                    msg += "(";
                    msg += x.c_str();
                    msg += ", ";
                    msg += y.c_str();
                    msg += ")";
                }
                EditorMessager::Ref()->add_message(msg);
            }
        });
    }

    ImGui::End();
}
void EditorToolsBar::process(){
}
void EditorToolsBar::post_process(){

}
void EditorToolsBar::draw(){

}


void EditorToolsBar::ui_init(EditorSpace* p_space){
    m_space = p_space;
}

void EditorToolsBar::m_update_shape(){
    m_shape = *(Rect2*)m_space;
    this->set_shape(m_shape);
}