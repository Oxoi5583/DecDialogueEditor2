
#include "core/random_server.h"
#include "engine/input_hub.h"
#include "engine/renderer.h"
#include "engine/texture_loader.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "graph/camera.h"
#include "graph/grid.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "struct/shape/rect2.h"
#include <cstddef>
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

#include "engine/window.h"
#include "theme/theme_loader.h"
#include "config/config_loader.h"
#include "core/timer_server.h"

int main(int argc, char* argv[]) {
    ThemeLoader::Ref()->load();
    ConfigLoader::Ref()->load();

    EngineWindow::Ref()->init();
    EngineRenderer::Ref()->init();

    EngineTextureLoader::Ref()->load();

    GraphCamera::Ref()->init();

    /*
    std::vector<vec2> random_points;
    std::vector<vec4> random_colors;
    for(size_t i = 0; i < 200; i++){
        vec2 new_pos = vec2(RandomServer::Ref()->get_uniform_distr_random_int(-5000, 5000),
                            RandomServer::Ref()->get_uniform_distr_random_int(-5000,5000));
        random_points.push_back(new_pos);

        vec4 new_color = vec4(
            RandomServer::Ref()->get_uniform_distr_random_float(0, 1),
            RandomServer::Ref()->get_uniform_distr_random_float(0, 1),
            RandomServer::Ref()->get_uniform_distr_random_float(0, 1),
            RandomServer::Ref()->get_uniform_distr_random_float(0, 1)
        );
        random_colors.push_back(new_color);
    }
    */
    std::vector<vec2> random_points;
    std::vector<vec2> random_size;
    std::vector<vec4> random_colors;
    for(size_t i = 0; i < 25000; i++){
        vec2 new_pos = vec2(RandomServer::Ref()->get_uniform_distr_random_int(-5000, 5000),
                            RandomServer::Ref()->get_uniform_distr_random_int(-5000,5000));
        random_points.push_back(new_pos);

        vec2 new_size = vec2(RandomServer::Ref()->get_uniform_distr_random_int(20, 100),
                            RandomServer::Ref()->get_uniform_distr_random_int(20,100));
        random_size.push_back(new_size);

        vec4 new_color = vec4(
            RandomServer::Ref()->get_uniform_distr_random_float(0, 0.3),
            RandomServer::Ref()->get_uniform_distr_random_float(0, 0.3),
            RandomServer::Ref()->get_uniform_distr_random_float(0, 0.3),
            RandomServer::Ref()->get_uniform_distr_random_float(0, 1)
        );
        random_colors.push_back(new_color);
    }


    Rect2 test_rect = {vec2(0.0f,0.0f), vec2(50.0f,50.0f)};
    bool is_test_dragging = false;

    while (EngineWindow::Ref()->is_running()){
        EngineInputHub::Ref()->polling();
        EngineWindow::Ref()->begin();
        EngineRenderer::Ref()->clear_draw_list();
        
        double delta = EngineWindow::Ref()->get_delta();
        EventServer::Ref()->flush();
        MouseServer::Ref()->update();
        GraphCamera::Ref()->update();
        TimerServer::Ref()->update(delta);

        GraphGrid::Ref()->draw();
        /*
        float radius = 500;
        vec2 target_pos = vec2(radius, 0);
        mat4 rotate_tras = glm::rotate(mat4(1.0f), (float)EngineWindow::Ref()->get_total_time() / 1000, vec3(0.0f,0.0f,1.0f));
        GraphCamera::Ref()->set_target(rotate_tras * vec4(target_pos,0.0f,1.0f));
        GraphCamera::Ref()->set_zoom(1.2f - cos((float)EngineWindow::Ref()->get_total_time() / 3000));
        */
        /*
        vec2 mouse_world_pos = EngineInputHub::Ref()->get_mouse_world_position();
        EngineRenderer::Ref()->draw_circle(vec2(0.0f,0.0f), 20, vec4(1.0f,1.0f,0,1.0f), 0);
        EngineRenderer::Ref()->draw_line(vec2(0.0f,0.0f), mouse_world_pos, vec4(0.0f,0.0f,0.5f,0.3f), 3);

        for(size_t i = 0; i < random_points.size(); i++){
            EngineRenderer::Ref()->draw_line(mouse_world_pos, random_points[i], random_colors[i], 2);
        }
        */
        
        for(size_t i = 0; i < random_points.size(); i++){
            EngineRenderer::Ref()->draw_rect(Rect2(random_points[i],random_size[i]), random_colors[i], 0);
        }
        

        vec2 mouse_pos = EngineInputHub::Ref()->get_mouse_world_position();
        if(EventServer::Ref()->has<EventMouseJustClickedOnWorld>()){
            auto event = EventServer::Ref()->poll_first<EventMouseJustClickedOnWorld>();
            vec2 clicked_pos = event.click_pos;

            if(test_rect.is_point_intersect(clicked_pos)){
                is_test_dragging = true;
            }
        }
        if(EventServer::Ref()->has<EventMouseReleased>()){
            is_test_dragging = false;
        }
        if(is_test_dragging){
            test_rect.set_center(mouse_pos);
        }
        vec4 color = (test_rect.is_point_intersect(mouse_pos)) ? vec4(1.0f,0.0f,0.0f,1.0f) : vec4(0.0f,1.0f,0.0f,1.0f);
        EngineRenderer::Ref()->draw_rect(test_rect, color, 0);

        EngineRenderer::Ref()->render();
        EngineWindow::Ref()->end();
    }

    EngineRenderer::Ref()->destory_all();
    EngineWindow::Ref()->destory_all();
    return 0;
}
