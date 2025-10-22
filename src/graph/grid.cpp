#include "graph/grid.h"
#include "DecToolsBox/debug/messenger.h"
#include "config/config_loader.h"
#include "engine/renderer.h"
#include "graph/camera.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"
#include <cstddef>
#include <vector>


GraphGrid::GraphGrid(){}
void GraphGrid::m_create_grid(){
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
void GraphGrid::init(){
    m_create_grid();
}
void GraphGrid::draw(){
    m_color = ThemeLoader::Ref()->get_color("SecondaryColour1");

    Rect2 window_rect = GraphCamera::Ref()->get_zoomed_rect();
    vec2 left_top = window_rect.get_left_top();
    vec2 right_down = window_rect.get_right_down();
    std::vector<GraphGridLine> lines = get_range_lines(
            left_top.x, left_top.y,
            right_down.x, right_down.y
        );

    
    for(GraphGridLine& line : lines){
        EngineRenderer::Ref()->draw_line(line.start, line.end, m_color, m_width);
    }
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
