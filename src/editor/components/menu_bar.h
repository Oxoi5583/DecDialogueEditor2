#pragma once

#include "server/timer_server.h"
#include "editor/components/base.h"
#include "editor/space.h"
#include "system/obj/abstract/clickable.h"
#include "struct/shape/rect2.h"

class EditorMenuBar : public EditorComponentBase{
private:
    EditorSpace* m_space = nullptr;
    Rect2 m_shape;
    TimerWrapper m_double_click_timer = {TimeUnit(TimeUnit::Type::MILLISECOND, 200) ,true};

    bool is_hover_any = false;
    void m_update_shape();
    void m_reset_vars();
    void m_begin_main_bar();
    void m_update_menu_file();
    void m_update_menu_edit();
    void m_update_menu_themes();
    void m_update_menu_language();
    void m_update_maximize_button();
    void m_update_minimize_button();
    void m_update_close_button();
    void m_end_main_bar();
    void m_handle_window();
    void m_block_resizer();
public:
    EditorMenuBar();
    ~EditorMenuBar();
    
    void ui_init(EditorSpace* p_space) override;

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};