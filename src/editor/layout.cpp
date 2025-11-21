#include "editor/layout.h"
#include "DecToolsBox/debug/messenger.h"
#include "editor/space.h"
#include "ext/debug/messenger_ext.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "graph/camera.h"
#include "graph/viewport.h"
#include "imgui/imgui.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"
#include <SDL3/SDL.h>



void EditorLayout::m_main_space_init(){
    vec2 cam_pos = GraphCamera::Ref()->get_target();
    vec2 window_size = EngineWindow::Ref()->get_window_size();

    m_main_space = {EditorSpace::SplitType::VERTICLE, cam_pos, window_size};
    m_main_space.from = EditorSpace::From::START;

    m_main_space.split_fixed.type = EditorSpace::SplitFixed::Type::VALUE;
    m_main_space.split_fixed.value = menu_bar_size;
    m_main_space.split_fixed.enable();

    m_main_space.split();

    m_menu_bar_space = m_main_space.get_children().first;
    m_menu_bar_other_space = m_main_space.get_children().second;

    m_menu_bar_other_space->set_type(EditorSpace::SplitType::VERTICLE);
    m_menu_bar_other_space->from = EditorSpace::From::START;
    m_menu_bar_other_space->split_fixed.type = EditorSpace::SplitFixed::Type::VALUE;
    m_menu_bar_other_space->split_fixed.value = tools_bar_size;
    m_menu_bar_other_space->split_fixed.enable();

    m_menu_bar_other_space->split();
    
    m_tools_bar_space = m_menu_bar_other_space->get_children().first;
    m_tools_bar_other_space = m_menu_bar_other_space->get_children().second;

    m_tools_bar_other_space->set_type(EditorSpace::SplitType::HORIZONTAL);
    m_tools_bar_other_space->from = EditorSpace::From::START;
    m_tools_bar_other_space->split_limit.min_type = EditorSpace::SplitLimit::Type::PROPORTION;
    m_tools_bar_other_space->split_limit.max_type = EditorSpace::SplitLimit::Type::PROPORTION;
    m_tools_bar_other_space->split_limit.min = 0.0f;
    m_tools_bar_other_space->split_limit.max = 0.7f;
    m_tools_bar_other_space->split_limit.enable();
    m_tools_bar_other_space->split_resizer.enable();
    m_tools_bar_other_space->split_magnets.push_back({0.0f,{0.0f,50.0f}, EditorSpace::SplitMagnet::Type::VALUE});
    m_tools_bar_other_space->split_magnets.push_back({100.0f,{50.0f,0.0f}, EditorSpace::SplitMagnet::Type::VALUE});
    
    m_tools_bar_other_space->split(0.2f);

    m_left_panel_space = m_tools_bar_other_space->get_children().first;
    m_left_panel_other_space = m_tools_bar_other_space->get_children().second;
}

void EditorLayout::m_main_space_update(){
    vec2 cam_pos = GraphCamera::Ref()->get_target();
    vec2 window_size = EngineWindow::Ref()->get_window_size();

    m_main_space.set_size(window_size);
    m_main_space.refresh_children();
    
    Rect2 screen_viewport_rect = *(Rect2*)get_world_space();
    vec2 screen_viewport_rect_lt = screen_viewport_rect.get_left_top() + (window_size / 2.0f);
    vec2 screen_viewport_rect_rd = screen_viewport_rect.get_right_down() + (window_size / 2.0f);
    
    GraphViewport::Ref()->set_viewport(
            screen_viewport_rect_lt.x, screen_viewport_rect_rd.x,
            screen_viewport_rect_lt.y, screen_viewport_rect_rd.y
        );

}

void EditorLayout::m_main_space_draw(){
    /*
    int i = 0;
    for(EditorSpace* s : m_main_space.get_spaces_ptr()){
        auto angles = s->to_world().get_points();
        for(size_t i = 0; i < angles.size(); i++){
            vec2 start = angles[i];
            vec2 end = (i == angles.size() - 1) ? angles[0] : angles[i + 1];
            
            EngineRenderer::Ref()->draw_line(start, end, vec4(0.0f, 0.0f, 0.0f, 1.0f), 3);
        }
        if(s->is_resizing()){
            EngineRenderer::Ref()->draw_rect(s->get_resizer_area().to_world(), vec4(0.0f, 1.0f, 0.0f, 0.5f), -1);
        }
        i++;
    }
    */
}

void EditorLayout::m_init_objs(){
    m_menu_bar = ObjectServer::Ref()->queue_create<EditorMenuBar>(ObjectServer::Layer::UI_LAYER);
    m_tools_bar = ObjectServer::Ref()->queue_create<EditorToolsBar>(ObjectServer::Layer::UI_LAYER);
    m_left_panel = ObjectServer::Ref()->queue_create<EditorLeftPanel>(ObjectServer::Layer::UI_LAYER);
}

void EditorLayout::ui_init(){
    refresh_theme();
    m_init_objs();
    m_main_space_init();

    m_menu_bar->ui_init(m_menu_bar_space);
    m_tools_bar->ui_init(m_tools_bar_space);
    m_left_panel->ui_init(m_left_panel_space);
}

void EditorLayout::ui_update(){
    m_main_space_update();

    int i = 0;
    for(EditorSpace* s : m_main_space.get_spaces_ptr()){
        if(s->has_children()){
            s->split();
            s->update_resizer();
        }
        i++;
    }
}

void EditorLayout::ui_draw(){
    m_main_space_draw();
}

vec2 EditorLayout::get_menu_bar_size(){
    return m_menu_bar->get_shape<Rect2>().get_size();
}

EditorSpace* EditorLayout::get_world_space(){
    return m_left_panel_other_space;
}
void EditorLayout::restore_layout(){
    for(EditorSpace* s : m_main_space.get_spaces_ptr()){
        if(s->has_children()){
            s->restore_buffer_value();
        }
    }
}

Rect2 EditorLayout::covnert_to_window(Rect2& p_rect){
    Rect2 ret = p_rect;
    vec2 new_pos = ret.get_position() + EngineWindow::Ref()->get_window_size() / 2.0f;
    ret.set_position(new_pos);
    return ret;
}

void EditorLayout::refresh_theme(){
    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    ImVec4 brand       = ThemeLoader::Ref()->get_imgui_color("BrandColour");
    ImVec4 secondary1  = ThemeLoader::Ref()->get_imgui_color("SecondaryColour1");
    ImVec4 secondary2  = ThemeLoader::Ref()->get_imgui_color("SecondaryColour2");
    ImVec4 secondary3  = ThemeLoader::Ref()->get_imgui_color("SecondaryColour3");
    ImVec4 accent1     = ThemeLoader::Ref()->get_imgui_color("AccentColour1");
    ImVec4 accent2     = ThemeLoader::Ref()->get_imgui_color("AccentColour2");
    ImVec4 textColor   = ThemeLoader::Ref()->get_imgui_color("TextColour");
    ImVec4 gridColor   = ThemeLoader::Ref()->get_imgui_color("GridColour");

    colors[ImGuiCol_WindowBg]          = secondary1;
    colors[ImGuiCol_ChildBg]           = secondary1;
    colors[ImGuiCol_PopupBg]           = secondary3;
    colors[ImGuiCol_Border]            = accent1;
    colors[ImGuiCol_BorderShadow]      = ImVec4(0, 0, 0, 0);

    colors[ImGuiCol_TitleBg]           = secondary2;
    colors[ImGuiCol_TitleBgActive]     = secondary3;
    colors[ImGuiCol_TitleBgCollapsed]  = secondary1;
    colors[ImGuiCol_MenuBarBg]         = secondary3;

    colors[ImGuiCol_FrameBg]           = secondary1;
    colors[ImGuiCol_FrameBgHovered]    = secondary2;
    colors[ImGuiCol_FrameBgActive]     = secondary3;

    colors[ImGuiCol_Button]            = secondary3;
    colors[ImGuiCol_ButtonHovered]     = secondary2;
    colors[ImGuiCol_ButtonActive]      = ImVec4(secondary2.x * 2.0f, secondary2.y * 2.0f, secondary2.z * 2.0f, 1.0f);

    colors[ImGuiCol_Header]            = secondary2;
    colors[ImGuiCol_HeaderHovered]     = accent2;
    colors[ImGuiCol_HeaderActive]      = ImVec4(accent2.x * 0.5f, accent2.y * 0.5f, accent2.z * 0.5f, 1.0f);

    colors[ImGuiCol_Tab]               = secondary1;
    colors[ImGuiCol_TabHovered]        = accent2;
    colors[ImGuiCol_TabActive]         = accent2;

    colors[ImGuiCol_Text]              = textColor;
    colors[ImGuiCol_TextDisabled]      = ImVec4(textColor.x * 0.5f, textColor.y * 0.5f, textColor.z * 0.5f, 1.0f);
    colors[ImGuiCol_TextSelectedBg]    = ImVec4(accent1.x, accent1.y, accent1.z, 0.35f);

    colors[ImGuiCol_SliderGrab]        = accent1;
    colors[ImGuiCol_SliderGrabActive]  = accent2;
    colors[ImGuiCol_Separator]         = accent1;
    colors[ImGuiCol_SeparatorHovered]  = accent2;
    colors[ImGuiCol_SeparatorActive]   = ImVec4(accent2.x * 0.5f, accent2.y * 0.5f, accent2.z * 0.5f, 1.0f);;

    colors[ImGuiCol_Border] = gridColor;

    EngineWindow::Ref()->set_clear_color({brand.x,brand.y,brand.z,brand.w});
}