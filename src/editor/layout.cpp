#include "editor/layout.h"
#include "DecToolsBox/debug/messenger.h"
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
    Rect2 cam_rect = GraphCamera::Ref()->get_zoomed_rect();

    m_main_space = {EditorSpace::SplitType::VERTICLE, cam_rect.get_position(), cam_rect.get_size()};
    m_main_space.from = EditorSpace::From::START;

    

    m_main_space.split_fixed.type = EditorSpace::SplitFixed::Type::VALUE;
    m_main_space.split_fixed.value = menu_bar_size;
    m_main_space.split_fixed.is_enabled = true;

    m_main_space.split();
}

void EditorLayout::m_main_space_update(){
    Rect2 cam_rect = GraphCamera::Ref()->get_zoomed_rect();

    m_main_space.set_size(cam_rect.get_size());
    m_main_space.set_position(cam_rect.get_position());
    m_main_space.refresh_children();
    
    vec2 window_size = EngineWindow::Ref()->get_window_size();
    GraphViewport::Ref()->set_viewport(0, window_size.x, 0, window_size.y);
    
    m_main_space.split();
}

void EditorLayout::m_main_space_draw(){
    int i = 0;
    for(EditorSpace s : m_main_space.get_spaces()){
        auto angles = s.get_points();
        for(size_t i = 0; i < angles.size(); i++){
            vec2 start = angles[i];
            vec2 end = (i == angles.size() - 1) ? angles[0] : angles[i + 1];
            EngineRenderer::Ref()->draw_line(start, end, vec4(0.0f, 0.0f, 0.0f, 1.0f), 3);
        }
        i++;
    }
}

void EditorLayout::m_init_objs(){
    m_menu_bar = ObjectServer::Ref()->queue_create<EditorMenuBar>(ObjectServer::Layer::UI_LAYER);
}

void EditorLayout::ui_init(){
    m_init_objs();

    m_main_space_init();
    m_menu_bar->ui_init(m_main_space.get_children().first);
}

void EditorLayout::ui_update(){
    m_main_space_update();
}

void EditorLayout::ui_draw(){
    m_main_space_draw();
}

vec2 EditorLayout::get_menu_bar_size(){
    return m_menu_bar->get_shape<Rect2>().get_size();
}