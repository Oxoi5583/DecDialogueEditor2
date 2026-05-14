#include "editor/components/lock_window.h"
#include <DecToolsBox/debug/messenger.h>
#include <editor/layout.h>
#include <editor/space.h>
#include <engine/window.h>
#include <glm/ext/vector_float2.hpp>
#include <imgui.h>
#include <server/event_server.h>
#include <server/events.h>
#include <struct/shape/rect2.h>
#include <system/graph/camera.h>
#include <system/graph/viewport.h>
#include <theme/theme_loader.h>

void EditorLockWindow::process(){
    this->m_opened = EventServer::Ref()->has<EventLockedAll>();
    EditorSpace* space = EditorLayout::Ref()->get_space("MENU_BAR_OTHER_SPACE");
    if(space && this->m_opened){
        Rect2 window_rect = EditorLayout::Ref()->covnert_to_window(*(Rect2*)space);
        vec2 size = window_rect.get_size();

        ImVec4 colour = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");
        vec2 pos = window_rect.get_left_top();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ThemeLoader::Ref()->ImVec4_to_int(colour));
        ImGui::SetNextWindowPos({pos.x, pos.y});
        ImGui::SetNextWindowSize({size.x, size.y});
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("EDITOR_LOCK_WINDOW", &this->m_opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav);
        ImGui::End();
        ImGui::PopStyleColor();
    }
}
