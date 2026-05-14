#include "DecToolsBox/debug/messenger.h"
#include "editor/components/up_coordinate.h"
#include "editor/layout.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "system/graph/camera.h"
#include "system/graph/grid.h"
#include "ext/debug/messenger_ext.h"
#include "system/graph/viewport.h"
#include "theme/theme_loader.h"
#include <string>

#include "ext/debug/messenger_ext.h"

void UpCoordinate::m_draw_background(){
    if(!m_is_freeze_mode){
        return;
    }

    ImVec4 bg_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 border_color = ImVec4(0.0f , 0.0f , 0.0f , 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, border_color);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

    vec2 window_size = EngineWindow::Ref()->get_window_size();


    ImGui::SetNextWindowPos({m_bg_left_top.x, m_bg_left_top.y});
    ImGui::SetNextWindowSize({m_bg_size.x ,m_bg_size.y});
    
    ImGui::Begin("UpCoordinate_Background", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::TextUnformatted("");
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}
#include <math.h>
void UpCoordinate::m_draw_blocks(){
    if(m_is_freeze_mode){
        return;
    }

    vec2 cam_left_top = GraphCamera::Ref()->get_left_top_buffer();

    vec2 leftest_pos = vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    ImVec4 bg_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 text_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    text_color = ImVec4(text_color.x + 0.3f, text_color.y + 0.3f, text_color.z + 0.3f, text_color.w);
    auto lines = GraphGrid::Ref()->get_shown_lines();

    ImGuiIO& io = ImGui::GetIO();

    for(auto line : lines){
        if(line.type == GraphGridLine::VERTICLE){
            ImVec4  applied_text_color = ImVec4(text_color.x, text_color.y, text_color.z, 0.8f);
            ImVec4 border_color = ImVec4(0.0f , 0.0f , 0.0f , 0.0f);
            if(fmod((float)line.id, 5.0f) != 0){
                applied_text_color.w -= 0.3f;
                io.FontGlobalScale = 0.85f;
            }else{
                io.FontGlobalScale = 1.0f;
            }

            ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);
            ImGui::PushStyleColor(ImGuiCol_Border, border_color);
            ImGui::PushStyleColor(ImGuiCol_Text, applied_text_color);



            ImGuiStyle& style = ImGui::GetStyle();
            float original_item_spacing_y = style.ItemSpacing.y;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, original_item_spacing_y));
            

            std::string name = "UpCoordinate_" + line.code;

            vec2 start_world_pos = vec2(line.start.x, cam_left_top.y);
            vec2 start_viewport_pos = GraphCamera::Ref()->world_to_viewport(start_world_pos);
            vec2 start_screen_pos = GraphViewport::Ref()->viewport_to_screen(start_viewport_pos);

            vec2 end_world_pos = start_world_pos + vec2(GraphGrid::Ref()->grid_interval, 0.0f);
            vec2 end_viewport_pos = GraphCamera::Ref()->world_to_viewport(end_world_pos);
            vec2 end_screen_pos = GraphViewport::Ref()->viewport_to_screen(end_viewport_pos);

            vec2 size = {end_screen_pos.x - start_screen_pos.x, height};
            vec2 pos = {start_screen_pos.x,start_screen_pos.y - height};
            float line_len = (fmod((float)line.id, 5.0f) != 0) ? height * 0.4f : height * 0.8f;
            float line_width = (fmod((float)line.id, 5.0f) != 0) ? 2.0f : 2.5f;

            if(pos.x < leftest_pos.x){
                leftest_pos = pos;
            }

            ImGui::SetNextWindowSize({size.x, size.y}); 
            ImGui::SetNextWindowPos({pos.x,pos.y});
            ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
            
                ImVec2 window_pos = ImGui::GetWindowPos();
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pos.x, window_pos.y + (height - line_len)), 
                                                            ImVec2(window_pos.x + line_width, window_pos.y + height + 6.0f),
                                                            ThemeLoader::Ref()->ImVec4_to_int(applied_text_color));

                if(MouseServer::Ref()->is_mouse_in_viewport() && !EventServer::Ref()->has<EventLockedAll>()){
                    if(start_screen_pos.x <= m_mouse_pos.x <= end_screen_pos.x){
                        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(m_mouse_pos.x, window_pos.y), 
                                                                ImVec2(m_mouse_pos.x + line_width, window_pos.y  + m_bg_size.x + 6.0f),
                                                                ThemeLoader::Ref()->ImVec4_to_int(text_color));
                    }
                }

                ImGui::SetCursorPos(ImVec2(5.0f, -2.5f));
                ImGui::TextUnformatted(line.code.c_str());
            ImGui::End();

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();
        }
    }

    ImVec4  applied_text_color = ImVec4(text_color.x, text_color.y, text_color.z, 0.8f);
    ImVec4 border_color = ImVec4(0.0f , 0.0f , 0.0f , 0.0f);
    ImGuiStyle& style = ImGui::GetStyle();
    float original_item_spacing_y = style.ItemSpacing.y;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, original_item_spacing_y));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, border_color);
    ImGui::PushStyleColor(ImGuiCol_Text, applied_text_color);
    vec2 pos = {m_bg_left_top.x, m_bg_left_top.y};
    vec2 size = {leftest_pos.x - m_bg_left_top.x, m_bg_size.y};
    ImGui::SetNextWindowSize({size.x, size.y});
    ImGui::SetNextWindowPos({pos.x, pos.y});
    ImGui::Begin("##UP_COORDINATE_OTHER_SPACE", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::End();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();


    io.FontGlobalScale = 1.0f;
}

void UpCoordinate::process(){
    vec2 window_size = EngineWindow::Ref()->get_window_size();
    m_space = EditorLayout::Ref()->get_up_coordinate_space();
    vec2 right_down = m_space->get_right_down() + (window_size / 2.0f);
    m_mouse_pos = MouseServer::Ref()->get_mouse_screen_position();
    m_bg_left_top = m_space->get_left_top() + (window_size / 2.0f);
    m_bg_size = right_down - m_bg_left_top;

    if(m_restore_time_fm_dragging_resizer_dlt < m_restore_time_fm_dragging_resizer){
        m_is_freeze_mode = true;
    }else{
        m_is_freeze_mode = false;
    }

    if(EventServer::Ref()->has<EventEditorSpaceResizerDragging>()){
        freeze();
    }else{
        m_restore_time_fm_dragging_resizer_dlt += EngineWindow::Ref()->get_delta();
    }

    m_draw_blocks();
    m_draw_background();
    ImGui::GetIO().FontGlobalScale = 1.0f;
}

void UpCoordinate::freeze(){
    m_restore_time_fm_dragging_resizer_dlt = 0.0f;
}