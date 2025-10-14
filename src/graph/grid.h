#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "imgui/imgui.h"
#include <cstdarg>
#include <cstdint>
#include <map>
#include <vector>

struct GraphGridLine{
    ImVec2 start;
    ImVec2 end;
};

class GraphGrid : public Singleton<GraphGrid>{
private:
    const double m_grid_interval = 15;
    const uint32_t m_grid_columns = 5000;
    const uint32_t m_grid_rows = 5000;
    std::map<float,GraphGridLine> m_vertical_lines;
    std::map<float,GraphGridLine> m_horizontal_lines;
public:
    void create_grid();
    std::vector<GraphGridLine> get_range_lines(float p_lefttop_x, float p_lefttop_y,
                                               float p_rightdown_x, float p_rightdown_y);
    void draw();
};