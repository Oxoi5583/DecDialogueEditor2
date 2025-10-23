#pragma once

#include "core/timer_server.h"
#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/abstract/clickable.h"
#include "obj/abstract/dragable.h"
#include "obj/abstract/hoverable.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"

using namespace glm;

class GraphNode : public DragableObject{
private:
    Timer* m_test_timer;

    Rect2& m_rect;
    int m_texture_id = 0;

    Rect2& m_init_shape();
public:
    GraphNode();
    ~GraphNode();

    vec2 get_position() const;
    vec2 get_size() const;
    void set_position(vec2& p_position);
    void set_size(vec2& p_size);

    bool is_point_intersect(vec2& p_point); 

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};
