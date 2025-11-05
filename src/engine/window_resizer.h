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

    std::map<Rect2*, EventMouseOnResizer::Direction> m_edges = {
        {&m_up_edge, EventMouseOnResizer::Direction::UP},
        {&m_down_edge, EventMouseOnResizer::Direction::DOWN},
        {&m_left_edge, EventMouseOnResizer::Direction::LEFT},
        {&m_right_edge, EventMouseOnResizer::Direction::RIGHT},
    };
    
    Circle m_left_top_angle;
    Circle m_left_down_angle;
    Circle m_right_top_angle;
    Circle m_right_down_angle;

    std::map<Circle*,EventMouseOnResizer::Direction>     m_angles = {
        {&m_left_top_angle, EventMouseOnResizer::Direction::UP_LEFT},
        {&m_left_down_angle, EventMouseOnResizer::Direction::DOWN_LEFT},
        {&m_right_top_angle, EventMouseOnResizer::Direction::UP_RIGHT},
        {&m_right_down_angle, EventMouseOnResizer::Direction::DOWN_RIGHT},
    };

    void m_refresh_resizer();
    void m_refresh_collision();
public:
    void init();
    void update();
};