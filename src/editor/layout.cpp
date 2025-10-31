#include "editor/layout.h"
#include "engine/window.h"
#include <SDL3/SDL.h>


void EditorLayout::init(){
    m_window_size_buffer = EngineWindow::Ref()->get_window_size();

}

void EditorLayout::update(){
    m_window_size_buffer = EngineWindow::Ref()->get_window_size();

}


