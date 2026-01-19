#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "editor/space.h"
#pragma once

class UpCoordinate : public Singleton<UpCoordinate>{
private:
    EditorSpace* m_space = nullptr;

    vec2 m_bg_left_top;
    vec2 m_bg_size;

    vec2 m_mouse_pos;
    
    void m_draw_background();
    void m_draw_blocks();

    double m_restore_time_fm_dragging_resizer = TimeUnit(TimeUnit::Type::SECOND, 0.5f).get_delta();
    double m_restore_time_fm_dragging_resizer_dlt = TimeUnit(TimeUnit::Type::SECOND, 0.5f).get_delta();
public:
    UpCoordinate() = default;
    ~UpCoordinate() = default;

    void process();

    const float height = 15;
};