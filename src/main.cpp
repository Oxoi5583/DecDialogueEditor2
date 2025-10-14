
#include "engine/event_hub.h"
#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "graph/camera.h"
#include "struct/rect2.h"
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


int main(int argc, char* argv[]) {
    ThemeLoader::Ref()->load();
    ConfigLoader::Ref()->load();

    EngineWindow::Ref()->init();
    EngineRenderer::Ref()->compile();

    GraphCamera::Ref()->init();

    while (EngineWindow::Ref()->is_running()){
        EngineEventHub::Ref()->polling();
        EngineWindow::Ref()->begin();

        GraphCamera::Ref()->update();
        
        EngineRenderer::Ref()->clear_draw_list();
        
        Rect2 r = Rect2(vec2(0,0),vec2(50,50));
        EngineRenderer::Ref()->draw_rect(r, vec4(255,0,0,255), 0);

        EngineRenderer::Ref()->draw_circle(r.get_left_top(), 5, vec4(0,0,255,255), 0);

        EngineRenderer::Ref()->draw_circle(GraphCamera::Ref()->get_origin(), 20, vec4(255,255,0,255), 0);
        EngineRenderer::Ref()->draw_circle(GraphCamera::Ref()->get_target(), 5, vec4(0,255,0,255), 0);

        vec2 mouse_pos = EngineEventHub::Ref()->get_mouse_world_position();
        DEBUG_MSG("mouse_pos : (" << mouse_pos.x << "," << mouse_pos.y << ")");
        EngineRenderer::Ref()->draw_circle(mouse_pos, 20, vec4(0,0,0,255), 0);

        GraphCamera::Ref()->set_target(GraphCamera::Ref()->get_target() - vec2(10,0));

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
    }

    EngineRenderer::Ref()->destory_all();
    EngineWindow::Ref()->destory_all();
    return 0;
}