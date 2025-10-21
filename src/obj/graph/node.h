#pragma once

#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include "struct/rect2.h"

using namespace glm;

class GraphNode : public ObjectBase{
private:
    Rect2 m_rect = {vec2(0.0f, 0.0f), vec2(50.0f, 50.0f)};
    int m_texture_id = 0;
public:
    GraphNode();
    ~GraphNode();

    vec2 get_position() const;
    vec2 get_size() const;
    void set_position(vec2& p_position);
    void set_size(vec2& p_size);

    bool is_point_intersect(vec2& p_point); 

    void ready(){}
    void pre_process(){}
    void process(){}
    void post_process(){}
    void draw();
};
