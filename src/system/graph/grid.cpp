#include "system/graph/grid.h"
#include "DecToolsBox/debug/messenger.h"
#include "config/config_loader.h"
#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "system/graph/camera.h"
#include "struct/shape/rect2.h"
#include "theme/theme_loader.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <queue>
#include <string>
#include <vector>

struct CharIdCalculator{
private:
    static constexpr size_t min = 1;
    static constexpr size_t max = 26;

    size_t cursor = 0;
    std::vector<size_t> data = {0}; 

    void carry(){
        cursor++;

        if(cursor + 1 > data.size()){
            data[cursor - 1] = min;
            data.push_back(1);
            cursor = 0;
            return;
        }

        data[cursor]++;
        if(data[cursor] <= max){
            cursor = 0;
            return;
        }else{
            data[cursor] = min;
            carry();
        }

        cursor = 0;
    }
public:
    void add(size_t p_val){
        for(size_t i = 0; i < p_val; i++){
            data[0]++;
            if(data[0] > max){
                data[0] = min;
                carry();
            }
        }
    }

    std::vector<size_t> get_data(){
        std::vector<size_t> ret = data;
        std::reverse(ret.begin(), ret.end());
        return ret;
    }
};

std::string get_char_id(int p_index){
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::vector<size_t> pending_char;
    
    int index = std::abs(p_index) + 1;

    CharIdCalculator calculator;
    calculator.add(index);
    std::vector<size_t> data = calculator.get_data();

    std::string ret = "";
    for(size_t it : data){
        ret += chars[it - 1];
    }

    if(p_index < 0){
        ret = "#" + ret;
    }

    return ret;
}

std::string get_int_id(int p_index){
    int index = std::abs(p_index);
    std::string ret = std::to_string(index);

    if(p_index < 0){
        ret = "#" + ret;
    }

    return ret;
}

GraphGrid::GraphGrid(){}
void GraphGrid::m_create_grid(){
    double size_x = grid_columns * grid_interval;
    double size_y = grid_rows * grid_interval;
    double start_x = - (size_x / 2);
    double start_y = - (size_y / 2);

    for(size_t y = 0; y < grid_columns; y++){
        double width = m_width;
        float fm_line_x = start_x;
        float fm_line_y = start_y + (y * grid_interval);
        float to_line_x = start_x + size_x;
        float to_line_y = start_y+ (y * grid_interval);

        bool is_skippable = false;
        if((y % 2) != 0){
            is_skippable = true;
        }

        if(!is_skippable){
            width = width * 1.5f;
        }else{
            width = width * 1.0f;
        }

        bool is_negative = (y - (grid_columns / 2.0f)) < 0 ? true : false;
        std::string line_id = get_int_id(y - (grid_columns / 2.0f));
        vec2 start = {round(fm_line_x),round(fm_line_y)};
        vec2 end = {round(to_line_x),round(to_line_y)};

        GraphGridLine line = {GraphGridLine::Type::HORIZONTAL, line_id, y, is_skippable, width, start, end,is_negative};
        m_horizontal_pos.emplace(line_id, start.y);
        m_horizontal_lines.emplace(fm_line_y,line);
    }
    for(size_t x = 0; x < grid_columns; x++){
        double width = m_width;
        float fm_line_x = start_x + (x * grid_interval);
        float fm_line_y = start_y;
        float to_line_x = start_x + (x * grid_interval);
        float to_line_y = start_y + size_y;

        bool is_skippable = false;
        if((x % 2) != 0){
            is_skippable = true;
        }

        if(!is_skippable){
            width = width * 1.5f;
        }else{
            width = width * 1.0f;
        }

        bool is_negative = (x - (grid_columns / 2.0f)) < 0 ? true : false;
        std::string line_id = get_char_id(x - (grid_columns / 2.0f));
        vec2 start = {round(fm_line_x),round(fm_line_y)};
        vec2 end = {round(to_line_x),round(to_line_y)};

        if(line_id.ends_with("ZZ")){
            DEBUG_MSG("line_id : " << line_id);
        }

        GraphGridLine line = {GraphGridLine::Type::VERTICLE, line_id, x, is_skippable, width, start, end,is_negative};
        m_vertical_pos.emplace(line_id, start.x);
        m_vertical_lines.emplace(fm_line_x,line);
    }
}
void GraphGrid::init(){
    m_create_grid();
}
void GraphGrid::draw(){
    double zoom = GraphCamera::Ref()->get_zoom();
    

    m_color = ThemeLoader::Ref()->get_color("GridColour");

    Rect2 window_rect = GraphCamera::Ref()->get_zoomed_rect();
    vec2 left_top = window_rect.get_left_top();
    vec2 right_down = window_rect.get_right_down();
    m_shown_lines_buf = get_range_lines(
            left_top.x, left_top.y,
            right_down.x, right_down.y
        );

    bool will_skip = false;
    if(m_shown_lines_buf.size() >= 100){
        will_skip = true;
    }

    for(size_t i = 0; i < m_shown_lines_buf.size(); i ++){
        GraphGridLine& line = m_shown_lines_buf[i];
        if(will_skip && line.is_skippable){
            continue;
        }

        EngineRenderer::Ref()->draw_line(line.start, line.end, m_color, ceil(line.width / std::min(zoom, 1.0)));
    }
}
std::vector<GraphGridLine> GraphGrid::get_shown_lines(){
    return m_shown_lines_buf;
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

bool GraphGrid::is_id_exists(std::string p_x, std::string p_y){
    if(m_vertical_pos.contains(p_x.c_str()) && m_horizontal_pos.contains(p_y.c_str())){
        return true;
    }
    return false;
}
vec2 GraphGrid::get_pos(std::string p_x, std::string p_y){
    if(!is_id_exists(p_x, p_y)){
        return vec2();
    }
    return {m_vertical_pos[p_x.c_str()], m_horizontal_pos[p_y.c_str()]};
}