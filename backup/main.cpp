
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

#include "SDL3/SDL_render.h"
#include "core/init.h"
#include "core/loop.h"
#include "theme/theme_loader.h"
#include "config/config_loader.h"


int main(int argc, char* argv[]) {
    ThemeLoader::Ref()->load();
    ConfigLoader::Ref()->load();

    SDL_Window* window = CoreInitializer::Ref()->get_sdl_window();
    SDL_Renderer* renderer = CoreInitializer::Ref()->get_sdl_renderer();
    ImGuiIO& io = CoreInitializer::Ref()->get_imgui_io();
    ImGuiStyle& style = CoreInitializer::Ref()->get_imgui_style();

    bool show_demo_window = true;
    bool show_another_window = false;

    float window_size_x = io.DisplaySize.x / 5;
    bool is_dragging = false;
    ImVec2 pos;
    ImVec2 size;

    bool done = false;
    while (CoreLoop::Ref()->poll()) {
        CoreLoop::Ref()->gl_clear();
        CoreLoop::Ref()->imgui_new_frame();


        ImGui::Begin("Node Editor");
        ImGui::SetWindowPos({0,0}); 
        ImGui::SetWindowSize(io.DisplaySize);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 offset = ImGui::GetCursorScreenPos();

        ImVec2 nodeA = ImVec2(offset.x + 100, offset.y + 100);
        ImVec2 nodeB = ImVec2(offset.x + 300, offset.y + 200);

        draw_list->AddRectFilled(nodeA, ImVec2(nodeA.x+80, nodeA.y+40), IM_COL32(80,120,255,255), 6.0f);
        draw_list->AddRectFilled(nodeB, ImVec2(nodeB.x+80, nodeB.y+40), IM_COL32(255,120,80,255), 6.0f);

        ImVec2 p1 = ImVec2(nodeA.x+80, nodeA.y+20);
        ImVec2 p2 = ImVec2(nodeB.x,   nodeB.y+20);
        draw_list->AddBezierCubic(p1, ImVec2(p1.x+50,p1.y),
                                    ImVec2(p2.x-50,p2.y),
                                    p2, IM_COL32(200,200,200,255), 2.0f);

        ImGui::End();

        ImVec4 clear_color = CoreLoop::Ref()->get_clear_color();



        ImGuiStyle& style = ImGui::GetStyle();
        //style.WindowRounding = 4.0f;
        //style.FrameRounding  = 3.0f;
        //style.GrabRounding   = 2.0f;
        style.WindowPadding  = ImVec2(8, 8);
        style.ItemSpacing    = ImVec2(6, 4);

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
        colors[ImGuiCol_WindowBg]        = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
        colors[ImGuiCol_Button]          = ImVec4(0.20f, 0.35f, 0.50f, 1.0f);
        colors[ImGuiCol_ButtonHovered]   = ImVec4(0.25f, 0.45f, 0.70f, 1.0f);
        colors[ImGuiCol_ButtonActive]    = ImVec4(0.15f, 0.30f, 0.50f, 1.0f);

        ImGui::Begin("Sidebar", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);
        if(ImGui::GetMousePos().x > pos.x + size.x){
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            if(ImGui::IsMouseDown(ImGuiMouseButton_Left)){
                is_dragging = true;
            }
        }else{
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            is_dragging = false;
        }
        if(is_dragging){
            window_size_x = ImGui::GetMousePos().x;
            DEBUG_MSG("size.x : " << size.x << " " << "window_size_x : " << window_size_x);
        }

        pos = ImGui::GetWindowPos();
        size = ImGui::GetWindowSize();
        ImGui::SetWindowSize(ImVec2(window_size_x, io.DisplaySize.y));
        ImGui::SetWindowPos(ImVec2(0, 0));


        ImGui::Text("Tools");
        ImGui::Separator();
        if (ImGui::Button("New Node")) { /* ... */ }

        ImGui::End();

        // Rendering
        CoreLoop::Ref()->render();
    }

    CoreInitializer::Ref()->shutdown();
    return 0;
}