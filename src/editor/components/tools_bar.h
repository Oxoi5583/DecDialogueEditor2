#pragma once

#include "server/timer_server.h"
#include "editor/components/base.h"
#include "editor/components/popup_window.h"
#include "editor/space.h"
#include "system/obj/abstract/clickable.h"
#include "struct/shape/rect2.h"

class EditorToolsBar : public EditorComponentBase{
private:
    EditorSpace* m_space = nullptr;
    Rect2 m_shape;
    TimerWrapper m_double_click_timer = {TimeUnit(TimeUnit::Type::MILLISECOND, 200) ,true};

    struct PopupWindowWrapper{
        std::string uid;
        PopupWindow* ptr;
    };

    PopupWindowWrapper m_go_to_window;

    void m_update_shape();
public:
    EditorToolsBar();
    ~EditorToolsBar();
    
    void ui_init(EditorSpace* p_space) override;

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};