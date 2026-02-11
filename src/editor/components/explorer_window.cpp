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
#include <iterator>
#include <string>

#include "ext/debug/messenger_ext.h"
#include "server/ui_icon_unicode.h"


ExplorerWindow::ExplorerWindow(){
    BIND_CLASS(ExplorerWindow);
}
ExplorerWindow::~ExplorerWindow(){

}

void ExplorerWindow::ready(){

}
void ExplorerWindow::pre_process(){
    vec2 engine_win_size = EngineWindow::Ref()->get_window_size();
    ImGui::SetNextWindowPos({engine_win_size.x, engine_win_size.y});
    ImGui::SetNextWindowSize({400.0f, 500.0f});
    ImGui::Begin("EXPLORER_WINDOW");
    ImGui::End();
}
void ExplorerWindow::process(){

}
void ExplorerWindow::post_process(){

}
void ExplorerWindow::draw(){

}
