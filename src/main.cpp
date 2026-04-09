#include "DecToolsBox/container/ordered_map.h"
#include "editor/components/explorer_window.h"
#include "editor/components/start_up_popup.h"
#include "engine/input_key.h"
#include "server/physics_server.h"
#include "server/project_server.h"
#include "server/ui_text_bank.h"
#include "editor/components/detail_window.h"
#include "editor/components/left_coordinate.h"
#include "editor/components/messager.h"
#include "editor/components/quick_text_display.h"
#include "editor/components/up_coordinate.h"
#include "editor/layout.h"
#include "editor/shortcut_menu.h"
#include "editor/space.h"
#include "engine/font_loader.h"
#include "engine/input_hub.h"
#include "engine/renderer.h"
#include "engine/texture_loader.h"
#include "engine/window_resizer.h"
#include "server/file_server.h"
#include "system/graph/background.h"
#include "system/graph/camera.h"
#include "system/graph/connection.h"
#include "system/graph/grid.h"
#include "system/graph/selection.h"
#include "system/graph/viewport.h"
#include "system/obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/line.h"
#include "struct/shape/rect2.h"
#include <cmath>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <server/mouse_server.h>
#include <server/timer_server.h>
#include <string>
#include <vector>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <DecToolsBox/debug/messenger.h>
#include "ext/debug/messenger_ext.h"

#include "engine/window.h"
#include "theme/theme_loader.h"
#include "config/config_loader.h"
#include "server/timer_server.h"

#include "DecToolsBox/core/condition.h"



int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, ".UTF-8");
    
    FileServer::Ref()->init();
    
    GraphGrid::Ref()->init();
    
    ThemeLoader::Ref()->load();
    ConfigLoader::Ref()->load();

    EngineWindow::Ref()->init();
    EngineRenderer::Ref()->init();
    EngineTextureLoader::Ref()->init();
    EngineInputHub::Ref()->init();
    EngineFontLoader::Ref()->init();

    GraphCamera::Ref()->init();
    GraphViewport::Ref()->init();
    GraphBackground::Ref()->init();
    GraphSelection::Ref()->init();
    EngineWindowResizer::Ref()->init();

    GraphManager::Ref()->init();
    
    EditorLayout::Ref()->ui_init();

    UiTextBank::Ref()->init();
    unsigned long long frame = 0;

    while (EngineWindow::Ref()->is_running()){
        EngineInputHub::Ref()->polling_sdl_event();

        EngineWindow::Ref()->begin();
        EngineRenderer::Ref()->clear_draw_list();
        
        if(frame == 0){
            ProjectServer::Ref()->init();
            EditorMessager::Ref()->init();
        }
        
        double delta = EngineWindow::Ref()->get_delta();

        ProjectServer::Ref()->process();
        StartupPopup::Ref()->process();
        QuickTextDisplay::Ref()->pre_process();
        
        GraphCamera::Ref()->update();
        TimerServer::Ref()->update(delta);
        ObjectServer::Ref()->clear_garbage();

        GraphViewport::Ref()->update();
        GraphConnection::Ref()->pre_update();
        GraphSelection::Ref()->pre_update();
        GraphManager::Ref()->update();
        EditorShortcutMenu::Ref()->update();
        EditorLayout::Ref()->ui_update();
        EngineWindowResizer::Ref()->update();
        EditorMessager::Ref()->process();

        GraphGrid::Ref()->draw();

        ObjectServer::Ref()->reorder();
        ObjectServer::Ref()->ready();
        ObjectServer::Ref()->pre_process();
        ObjectServer::Ref()->process();
        ObjectServer::Ref()->post_process();
        ObjectServer::Ref()->draw();
        
        GraphBackground::Ref()->update();
        GraphConnection::Ref()->post_update();
        GraphSelection::Ref()->post_update();
        GraphSelection::Ref()->draw();
        

        EngineWindowResizer::Ref()->post_update();
        MouseServer::Ref()->update();
        EditorLayout::Ref()->ui_draw();
        UpCoordinate::Ref()->process();
        LeftCoordinate::Ref()->process();
        QuickTextDisplay::Ref()->process();
        EditorMessager::Ref()->draw();

        if(frame == 0){
            GraphCamera::Ref()->refresh_left_top_buffer();
        }

        EventServer::Ref()->flush();

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
        
        PhysicsServer::Ref()->update();
        
        if(EngineInputHub::Ref()->keyboard_is_down(K_LCTRL) && MouseServer::Ref()->is_clicked()){
            EventSpawnNode event;
            event.spawn_pos = MouseServer::Ref()->get_mouse_world_position();
            event.type = GraphManager::NodeTypeId::NODE;
            EventServer::Ref()->emit(event);
        }

        frame++;
    }
    
    ProjectServer::Ref()->shutdown();
    MouseServer::Ref()->shutdown();
    ObjectServer::Ref()->shutdown();
    TimerServer::Ref()->shutdown();

    EngineRenderer::Ref()->shutdown();
    EngineWindow::Ref()->shutdown();

    return 0;
}
