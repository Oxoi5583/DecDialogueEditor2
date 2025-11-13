#pragma once

#include "core/timer_server.h"
#include "editor/components/base.h"
#include "editor/space.h"
#include "obj/abstract/clickable.h"
#include "struct/shape/rect2.h"

class EditorLeftPanel : public EditorComponentBase{
private:
    EditorSpace* m_space = nullptr;
    Rect2 m_shape;
    Timer* m_double_click_timer = nullptr;

    int m_current_item = -1;


    void m_update_shape();
public:
    EditorLeftPanel();
    ~EditorLeftPanel();
    
    void ui_init(EditorSpace* p_space) override;

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};