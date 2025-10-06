
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

    bool done = false;
    while (CoreLoop::Ref()->poll()) {
        CoreLoop::Ref()->new_frame();    
        ImVec4 clear_color = CoreLoop::Ref()->get_clear_color();


        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 4.0f;
        style.FrameRounding  = 3.0f;
        style.GrabRounding   = 2.0f;
        style.WindowPadding  = ImVec2(8, 8);
        style.ItemSpacing    = ImVec2(6, 4);

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
        colors[ImGuiCol_WindowBg]        = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);
        colors[ImGuiCol_Button]          = ImVec4(0.20f, 0.35f, 0.50f, 1.0f);
        colors[ImGuiCol_ButtonHovered]   = ImVec4(0.25f, 0.45f, 0.70f, 1.0f);
        colors[ImGuiCol_ButtonActive]    = ImVec4(0.15f, 0.30f, 0.50f, 1.0f);


        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 5, io.DisplaySize.y));
        ImGui::Begin("Sidebar", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);

        ImGui::Text("Tools");
        ImGui::Separator();
        if (ImGui::Button("New Node")) { /* ... */ }

    ImGui::End();
        // Rendering
        CoreLoop::Ref()->render();
    }

    CoreInitializer::Ref()->destory_all();
    return 0;
}