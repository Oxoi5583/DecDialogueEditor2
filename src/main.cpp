#include "editor/space.h"
#include "engine/input_hub.h"
#include "engine/renderer.h"
#include "engine/texture_loader.h"
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

    GraphGrid::Ref()->init();
    GraphCamera::Ref()->init();
    GraphViewport::Ref()->init();

    GraphManager* graph_manager = ObjectServer::Ref()->queue_create<GraphManager>();

    Timer* test_timer = TimerServer::Ref()->create_timer({TimeUnit::Type::SECOND, 1}, false);
    test_timer->start();

    Rect2 cam_rect = GraphCamera::Ref()->get_zoomed_rect();
    EditorSpace space = {EditorSpace::SplitType::VERTICLE, cam_rect.get_position(), cam_rect.get_size()};
    space.split(0.5);

    while (EngineWindow::Ref()->is_running()){
        EngineInputHub::Ref()->polling_sdl_event();

        EngineWindow::Ref()->begin();
        EngineRenderer::Ref()->clear_draw_list();
        
        double delta = EngineWindow::Ref()->get_delta();

        MouseServer::Ref()->update();
        GraphCamera::Ref()->update();
        GraphViewport::Ref()->update();
        TimerServer::Ref()->update(delta);
        ObjectServer::Ref()->clear_garbage();

        GraphGrid::Ref()->draw();

        ObjectServer::Ref()->ready();
        ObjectServer::Ref()->pre_process();
        ObjectServer::Ref()->process();
        ObjectServer::Ref()->post_process();
        ObjectServer::Ref()->draw();

        if(test_timer->timeout_and_reset_in_cycle(20)){
            EventSpawnNode event;
            event.spawn_pos = {test_timer->get_current_cycle() * 10,0.0f};
            EventServer::Ref()->emit(event);
        }

        Rect2 cam_rect = GraphCamera::Ref()->get_zoomed_rect();
        space.set_size(cam_rect.get_size());
        space.set_position(cam_rect.get_position());
        space.refresh_children();
        space.get_children().first->split(0.5);
        space.get_children().second->split(0.5);
        space.get_children().first->set_type(EditorSpace::SplitType::HORIZONTAL);
        space.get_children().second->set_type(EditorSpace::SplitType::HORIZONTAL);
        space.get_children().first->get_children().first->split(0.3);
        space.get_children().second->get_children().first->split(0.8);
        space.get_children().first->get_children().second->split(0.3);
        space.get_children().second->get_children().second->split(0.8);
        space.get_children().first->get_children().first->set_type(EditorSpace::SplitType::HORIZONTAL);
        space.get_children().second->get_children().first->set_type(EditorSpace::SplitType::HORIZONTAL);
        space.get_children().first->get_children().second->set_type(EditorSpace::SplitType::HORIZONTAL);
        space.get_children().second->get_children().second->set_type(EditorSpace::SplitType::HORIZONTAL);
        /*
        double proportion = space.get_proportion(MouseServer::Ref()->get_mouse_world_position());
        space.split(proportion);
        */

        int i = 0;
        for(EditorSpace s : space.get_spaces()){
            EngineRenderer::Ref()->draw_rect(s, {0.0f,0.0f,0.0f,1.0f}, 0);
            i++;
        }

        EventServer::Ref()->flush();

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
    }

    EngineRenderer::Ref()->destory_all();
    EngineWindow::Ref()->destory_all();
    return 0;
}
