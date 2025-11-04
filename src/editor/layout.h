#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/container/ordered_list.h"
#include "editor/components/base.h"
#include "editor/components/menu_bar.h"
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

    void m_init_objs();

    EditorMenuBar* m_menu_bar;
public:
    EditorLayout(){}
    ~EditorLayout(){}

    vec2 get_menu_bar_size();

    void ui_init();
    void ui_update();
    void ui_draw();
    
    const double menu_bar_size = 30;
};