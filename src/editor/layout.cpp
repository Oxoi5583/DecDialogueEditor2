#include "editor/layout.h"
#include "DecToolsBox/debug/messenger.h"
#include "editor/space.h"
#include "ext/debug/messenger_ext.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "graph/camera.h"
#include "graph/viewport.h"
#include "server/mouse_server.h"
#include "server/object_server.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
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

    m_menu_bar_other_space->set_type(EditorSpace::SplitType::HORIZONTAL);
    m_menu_bar_other_space->from = EditorSpace::From::START;
    m_menu_bar_other_space->split_limit.type = EditorSpace::SplitLimit::Type::PROPORTION;
    m_menu_bar_other_space->split_limit.min = 0.05f;
    m_menu_bar_other_space->split_limit.max = 0.3f;
    m_menu_bar_other_space->split_limit.enable();
    m_menu_bar_other_space->split_resizer.enable();
    
    m_menu_bar_other_space->split(0.2f);
    
    m_left_panel_space = m_menu_bar_other_space->get_children().first;
    m_left_panel_other_space = m_menu_bar_other_space->get_children().second;
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

    m_main_space.split();
    m_menu_bar_other_space->split();
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
    }*/
}

void EditorLayout::m_init_objs(){
    m_menu_bar = ObjectServer::Ref()->queue_create<EditorMenuBar>(ObjectServer::Layer::UI_LAYER);
}

void EditorLayout::ui_init(){
    m_init_objs();

    m_main_space_init();
    m_menu_bar->ui_init(m_menu_bar_space);
}

void EditorLayout::ui_update(){
    m_main_space_update();

    int i = 0;
    for(EditorSpace* s : m_main_space.get_spaces_ptr()){
        s->update_resizer();
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