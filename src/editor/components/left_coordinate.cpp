#include "DecToolsBox/debug/messenger.h"
#include "editor/components/left_coordinate.h"
#include "editor/layout.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "imgui/imgui.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "system/graph/camera.h"
#include "system/graph/grid.h"
#include "ext/debug/messenger_ext.h"
#include "system/graph/viewport.h"
#include "theme/theme_loader.h"
#include <string>

#include "ext/debug/messenger_ext.h"

void LeftCoordinate::m_draw_background(){
    m_space = EditorLayout::Ref()->get_left_coordinate_space();

    ImVec4 bg_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 border_color = ImVec4(0.0f , 0.0f , 0.0f , 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);
    ImGui::PushStyleColor(ImGuiCol_Border, border_color);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

    vec2 window_size = EngineWindow::Ref()->get_window_size();

    m_bg_left_top = m_space->get_left_top() + (window_size / 2.0f);
    vec2 right_down = m_space->get_right_down() + (window_size / 2.0f);
    m_bg_size = right_down - m_bg_left_top;

    ImGui::SetNextWindowPos({m_bg_left_top.x, m_bg_left_top.y});
    ImGui::SetNextWindowSize({m_bg_size.x ,m_bg_size.y});
    
    ImGui::Begin("LeftCoordinate_Background", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::TextUnformatted("");
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();
}
#include <math.h>
void LeftCoordinate::m_draw_blocks(){
    if(m_restore_time_fm_dragging_resizer_dlt < m_restore_time_fm_dragging_resizer){
        return;
    }

    float max_width = 0.0f;

    vec2 cam_left_top = GraphCamera::Ref()->get_left_top_buffer();

    ImVec4 bg_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 text_color = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    text_color = ImVec4(text_color.x + 0.3f, text_color.y + 0.3f, text_color.z + 0.3f, text_color.w);
    auto lines = GraphGrid::Ref()->get_shown_lines();

    ImGuiIO& io = ImGui::GetIO();

    for(auto line : lines){
        if(line.type == GraphGridLine::HORIZONTAL){
            float applied_width = m_bg_size.x
            ;
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
            

            std::string name = "LeftCoordinate_" + line.code;

            vec2 start_world_pos = vec2(cam_left_top.x, line.start.y);
            vec2 start_viewport_pos = GraphCamera::Ref()->world_to_viewport(start_world_pos);
            vec2 start_screen_pos = GraphViewport::Ref()->viewport_to_screen(start_viewport_pos);

            vec2 end_world_pos = start_world_pos + vec2(0.0f, GraphGrid::Ref()->grid_interval);
            vec2 end_viewport_pos = GraphCamera::Ref()->world_to_viewport(end_world_pos);
            vec2 end_screen_pos = GraphViewport::Ref()->viewport_to_screen(end_viewport_pos);

            float line_len = (fmod((float)line.id, 5.0f) != 0) ? applied_width * 0.4f : applied_width * 0.8f;
            float line_width = (fmod((float)line.id, 5.0f) != 0) ? 2.0f : 2.5f;

            ImGui::SetNextWindowSize({m_bg_size.x, end_screen_pos.y - start_screen_pos.y}); 
            ImGui::SetNextWindowPos({m_bg_left_top.x,start_screen_pos.y});
            ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs);
                ImVec2 window_pos = ImGui::GetWindowPos();
                ImGui::SetCursorPos(ImVec2(5.0f, 0.0f));
                ImGui::TextUnformatted(line.code.c_str());


                
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pos.x + (applied_width - line_len), window_pos.y), 
                                                            ImVec2(window_pos.x + applied_width + 6.0f, window_pos.y + line_width),
                                                            ThemeLoader::Ref()->ImVec4_to_int(applied_text_color));

                if(MouseServer::Ref()->is_mouse_in_viewport()){
                    if(start_screen_pos.y <= m_mouse_pos.y <= end_screen_pos.y){
                        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(window_pos.x, m_mouse_pos.y), 
                                                                ImVec2(window_pos.x + m_bg_size.y + 6.0f, m_mouse_pos.y + line_width),
                                                                ThemeLoader::Ref()->ImVec4_to_int(text_color));
                    }
                }

                float code_width = ImGui::CalcTextSize(line.code.c_str()).x;
                if(code_width > max_width){
                    max_width = code_width;
                }
            ImGui::End();
            
    

            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar();

        }
    }

    width = (max_width <= 20) ? 20.0f : max_width + 7.0f;
    io.FontGlobalScale = 1.0f;
}

void LeftCoordinate::process(){
    m_mouse_pos = MouseServer::Ref()->get_mouse_screen_position();

    if(EventServer::Ref()->has<EventEditorSpaceResizerDragging>()){
        m_restore_time_fm_dragging_resizer_dlt = 0.0f;
    }else{
        m_restore_time_fm_dragging_resizer_dlt += EngineWindow::Ref()->get_delta();
    }

    m_draw_background();
    m_draw_blocks();
}