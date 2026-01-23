#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "editor/space.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"
#include "imgui/imgui.h"
#include <cstdarg>
#include <cstdint>
#include <map>
#include <vector>

using namespace glm;

struct GraphGridLine{
    enum Type{
        HORIZONTAL,
        VERTICLE
    };

    Type type;
    std::string code;
    size_t id;
    bool is_skippable;
    double width;
    vec2 start;
    vec2 end;
    bool is_negative;
};

class GraphGrid : public Singleton<GraphGrid>{
private:
    vec4 m_color = vec4(0.0f,0.0f,0.0f,1.0f);
    float m_width = 1;

    std::map<float,GraphGridLine> m_vertical_lines;
    std::map<float,GraphGridLine> m_horizontal_lines;
    void m_create_grid();

    std::map<std::string, float> m_vertical_pos;
    std::map<std::string, float> m_horizontal_pos;

    std::vector<GraphGridLine> m_shown_lines_buf;
public:
    GraphGrid();
    std::vector<GraphGridLine> get_range_lines(float p_lefttop_x, float p_lefttop_y,
                                               float p_rightdown_x, float p_rightdown_y);
    
    std::vector<GraphGridLine> get_shown_lines();
    void init();
    void draw();

    const double grid_interval = 25;
    const uint32_t grid_columns = 50000;
    const uint32_t grid_rows = 50000;

    bool is_id_exists(std::string p_x, std::string p_y);
    vec2 get_pos(std::string p_x, std::string p_y);
};