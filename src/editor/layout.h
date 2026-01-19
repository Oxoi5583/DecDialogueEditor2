#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/container/ordered_list.h"
#include "editor/components/base.h"
#include "editor/components/left_panel.h"
#include "editor/components/menu_bar.h"
#include "editor/components/tools_bar.h"
#include "editor/space.h"
#include "glm/ext/scalar_int_sized.hpp"
#include "glm/ext/scalar_uint_sized.hpp"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"
#include <map>
#include <memory>
#include <vector>

using namespace glm;

class EditorLayout : public Singleton<EditorLayout>{
private:
    EditorSpace m_main_space;
    void m_main_space_init();
    void m_main_space_update();
    void m_main_space_draw();

    EditorSpace* m_menu_bar_space;
    EditorSpace* m_menu_bar_other_space;
    
    EditorSpace* m_tools_bar_space;
    EditorSpace* m_tools_bar_other_space;

    EditorSpace* m_left_panel_space;
    EditorSpace* m_left_panel_other_space;

    EditorSpace* m_up_coordinate_space;
    EditorSpace* m_up_coordinate_other_space;
    EditorSpace* m_left_coordinate_space;
    EditorSpace* m_left_coordinate_other_space;
    void m_left_coordinate_space_width_update();

    void m_init_objs();

    EditorMenuBar* m_menu_bar;
    EditorToolsBar* m_tools_bar;
    EditorLeftPanel* m_left_panel;
public:
    EditorLayout(){}
    ~EditorLayout(){}

    vec2 get_menu_bar_size();

    void ui_init();
    void ui_update();
    void ui_draw();
    
    const double menu_bar_size = 30;
    const double tools_bar_size = 45;

    EditorSpace* get_up_coordinate_space();
    EditorSpace* get_left_coordinate_space();
    EditorSpace* get_world_space();
    void restore_layout();

    Rect2 covnert_to_window(Rect2& p_rect);

    void refresh_theme();
};