#include "editor/layout.h"
#include "editor/space.h"
#include "engine/font_loader.h"
#include "engine/input_hub.h"
#include "engine/renderer.h"
#include "engine/texture_loader.h"
#include "engine/window_resizer.h"
#include "graph/background.h"
#include "graph/camera.h"
#include "graph/grid.h"
#include "graph/viewport.h"
#include "obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include <cmath>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
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
#include "core/timer_server.h"

int main(int argc, char* argv[]) {
    ThemeLoader::Ref()->load();
    ConfigLoader::Ref()->load();

    EngineWindow::Ref()->init();
    EngineRenderer::Ref()->init();
    EngineTextureLoader::Ref()->init();
    EngineFontLoader::Ref()->init();
    EngineInputHub::Ref()->init();

    GraphGrid::Ref()->init();
    GraphCamera::Ref()->init();
    GraphViewport::Ref()->init();
    EngineWindowResizer::Ref()->init();

    GraphManager* graph_manager = ObjectServer::Ref()->queue_create<GraphManager>();

    Timer* test_timer = TimerServer::Ref()->create_timer({TimeUnit::Type::SECOND, 5}, false);
    test_timer->start();
    
    EditorLayout::Ref()->ui_init();

    DEBUG_MSG("End Init.");

    while (EngineWindow::Ref()->is_running()){
        EngineInputHub::Ref()->polling_sdl_event();

        EngineWindow::Ref()->begin();
        EngineRenderer::Ref()->clear_draw_list();
        
        double delta = EngineWindow::Ref()->get_delta();

        GraphCamera::Ref()->update();
        TimerServer::Ref()->update(delta);
        ObjectServer::Ref()->clear_garbage();

        EditorLayout::Ref()->ui_update();
        GraphViewport::Ref()->update();
        EngineWindowResizer::Ref()->update();

        GraphGrid::Ref()->draw();

        ObjectServer::Ref()->ready();
        ObjectServer::Ref()->pre_process();
        ObjectServer::Ref()->process();
        ObjectServer::Ref()->post_process();
        ObjectServer::Ref()->draw();
        GraphBackground::Ref()->update();

        if(test_timer->timeout_and_reset()){
            EventSpawnNode event;
            event.spawn_pos = {test_timer->get_current_cycle() * 10,0.0f};
            EventServer::Ref()->emit(event);
            EngineWindow::Ref()->stop_dragging();
            test_timer->stop();
        }

        MouseServer::Ref()->update();
        EditorLayout::Ref()->ui_draw();

        EventServer::Ref()->flush();

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
    }

    EngineRenderer::Ref()->destory_all();
    EngineWindow::Ref()->destory_all();
    return 0;
}
