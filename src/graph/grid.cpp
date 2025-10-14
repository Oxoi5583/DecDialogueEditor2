#include "graph/grid.h"
#include <cstddef>

void GraphGrid::create_grid(){
    double size_x = m_grid_columns * m_grid_interval;
    double size_y = m_grid_rows * m_grid_interval;
    double start_x = - (size_x / 2);
    double start_y = - (size_y / 2);

    for(size_t y = 0; y < m_grid_columns; y++){
        float fm_line_x = start_x;
        float fm_line_y = start_y + (y * m_grid_interval);
        float to_line_x = start_x + size_x;
        float to_line_y = start_y+ (y * m_grid_interval);
        GraphGridLine line = {{fm_line_x,fm_line_y},{to_line_x,to_line_y}};
        m_horizontal_lines.emplace(fm_line_y,line);
    }
    for(size_t x = 0; x < m_grid_columns; x++){
        float fm_line_x = start_x + (x * m_grid_interval);
        float fm_line_y = start_y;
        float to_line_x = start_x + (x * m_grid_interval);
        float to_line_y = start_y + size_y;
        GraphGridLine line = {{fm_line_x,fm_line_y},{to_line_x,to_line_y}};
        m_vertical_lines.emplace(fm_line_x,line);
    }
}

void GraphGrid::draw(){
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    
}

std::vector<GraphGridLine> GraphGrid::get_range_lines(float p_lefttop_x, float p_lefttop_y,
                                                      float p_rightdown_x, float p_rightdown_y){
    std::vector<GraphGridLine> ret;
    auto vertical_begin = m_vertical_lines.lower_bound(p_lefttop_x);
    auto vertical_end = m_vertical_lines.upper_bound(p_rightdown_x);
    auto horizontal_begin = m_horizontal_lines.lower_bound(p_lefttop_y);
    auto horizontal_end = m_horizontal_lines.upper_bound(p_rightdown_y);

    ret.reserve(
        std::distance(vertical_begin, vertical_end) +
        std::distance(horizontal_begin, horizontal_end)
    );

    for (auto it = vertical_begin; it != vertical_end; ++it){
        ret.push_back(it->second);
    }
    for (auto it = horizontal_begin; it != horizontal_end; ++it){
        ret.push_back(it->second);
    }
    return ret;
}
