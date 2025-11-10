#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "engine/window.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "struct/shape/Circle.h"
#include "struct/shape/rect2.h"
#include <functional>
#include <map>

class EngineWindowResizer : public Singleton<EngineWindowResizer>{
private:
    Rect2 m_up_edge;
    Rect2 m_down_edge;
    Rect2 m_left_edge;
    Rect2 m_right_edge;

    std::map<Rect2*, EventDirection> m_edges = {
        {&m_up_edge, EventDirection::UP},
        {&m_down_edge, EventDirection::DOWN},
        {&m_left_edge, EventDirection::LEFT},
        {&m_right_edge, EventDirection::RIGHT},
    };
    
    Circle m_left_top_angle;
    Circle m_left_down_angle;
    Circle m_right_top_angle;
    Circle m_right_down_angle;

    std::map<Circle*,EventDirection>     m_angles = {
        {&m_left_top_angle, EventDirection::UP_LEFT},
        {&m_left_down_angle, EventDirection::DOWN_LEFT},
        {&m_right_top_angle, EventDirection::UP_RIGHT},
        {&m_right_down_angle, EventDirection::DOWN_RIGHT},
    };

    bool m_is_dragging = false;
    EventDirection m_dragging_dir;

    void m_refresh_resizer();
    void m_refresh_collision();
    void m_refresh_is_dragging();
    void m_refresh_emit_dragging_event();
public:
    void init();
    void update();
    void post_update();
};