#include "engine/input_hub.h"
#include "engine/renderer.h"
#include "engine/texture_loader.h"
#include "graph/camera.h"
#include "graph/grid.h"
#include "obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl3.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <DecToolsBox/debug/messenger.h>

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
    
    GraphManager* graph_manager = ObjectServer::Ref()->queue_create<GraphManager>();

    Timer* test_timer = TimerServer::Ref()->create_timer({TimeUnit::Type::SECOND, 1}, false);
    test_timer->start();

    while (EngineWindow::Ref()->is_running()){
        EngineInputHub::Ref()->polling_sdl_event();

        EngineWindow::Ref()->begin();
        EngineRenderer::Ref()->clear_draw_list();
        
        double delta = EngineWindow::Ref()->get_delta();

        MouseServer::Ref()->update();
        GraphCamera::Ref()->update();
        TimerServer::Ref()->update(delta);
        ObjectServer::Ref()->clear_garbage();

        GraphGrid::Ref()->draw();

        ObjectServer::Ref()->ready();
        ObjectServer::Ref()->pre_process();
        ObjectServer::Ref()->process();
        ObjectServer::Ref()->post_process();
        ObjectServer::Ref()->draw();

        if(test_timer->timeout_and_reset_in_cycle(3)){
            DEBUG_MSG("Spawn!!");
            EventSpawnNode event;
            event.spawn_pos = {test_timer->get_current_cycle() * 10,0.0f};
            EventServer::Ref()->emit(event);
        }

        if(EventServer::Ref()->has<EventMouseHoverObj>()){
            EventMouseHoverObj event = EventServer::Ref()->poll_first<EventMouseHoverObj>();
            if(event.is_event_occurred){
                vec2 pos = EventServer::Ref()->poll_first<EventMouseHoverObj>().hovering_pos;
                DEBUG_MSG("Hovering " << event.obj_id);
                EngineRenderer::Ref()->draw_circle(event.hovering_pos, 5.0f, vec4(0.0f,1.0f,1.0f,1.0f), 0);
                EngineRenderer::Ref()->draw_rect({event.hovering_pos, vec2(15.0f,15.0f)}, vec4(0.0f,1.0f,1.0f,1.0f), 0);
            }
        }

        EventServer::Ref()->flush();

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
    }

    EngineRenderer::Ref()->destory_all();
    EngineWindow::Ref()->destory_all();
    return 0;
}
