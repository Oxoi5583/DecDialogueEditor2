
#include "engine/event_hub.h"
#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
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
    GraphCamera::Ref()->set_zoom(0.1);
    while (EngineWindow::Ref()->is_running()){
        EngineEventHub::Ref()->polling();
        EngineWindow::Ref()->begin();

        GraphCamera::Ref()->update();
        
        EngineRenderer::Ref()->clear_draw_list();
        
        /*
        Rect2 r = Rect2(vec2(0,0),vec2(50,50));
        EngineRenderer::Ref()->draw_rect(r, vec4(255,0,0,255), 0);

        EngineRenderer::Ref()->draw_circle(r.get_left_top(), 5, vec4(0,0,255,255), 0);

        EngineRenderer::Ref()->draw_circle(GraphCamera::Ref()->get_origin(), 20, vec4(255,255,0,255), 0);
        EngineRenderer::Ref()->draw_circle(GraphCamera::Ref()->get_target(), 5, vec4(0,255,0,255), 0);

        vec2 mouse_world_pos = EngineEventHub::Ref()->get_mouse_world_position();
        //DEBUG_MSG("mouse_world_pos(world) : (" << mouse_world_pos.x << "," << mouse_world_pos.y << ")");
        EngineRenderer::Ref()->draw_circle(mouse_world_pos, 20, vec4(0,0,0,255), 0);
        vec2 mouse_screen_pos = GraphCamera::Ref()->world_to_screen(mouse_world_pos);
        //DEBUG_MSG("mouse_world_pos(screen) : (" << mouse_screen_pos.x << "," << mouse_screen_pos.y << ")");
        */

        vec2 mouse_world_pos = EngineEventHub::Ref()->get_mouse_world_position();
        EngineRenderer::Ref()->draw_line(vec2(0.0f,0.0f), mouse_world_pos, vec4(0,0,0,255));
    
        /*
        float radius = 150;
        vec2 target_pos = vec2(radius, 0);
        mat4 rotate_tras = glm::rotate(mat4(1.0f), (float)EngineWindow::Ref()->get_total_time() / 200, vec3(0.0f,0.0f,1.0f));
        GraphCamera::Ref()->set_target(rotate_tras * vec4(target_pos,0.0f,1.0f));
        */

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
    }

    EngineRenderer::Ref()->destory_all();
    EngineWindow::Ref()->destory_all();
    return 0;
}
