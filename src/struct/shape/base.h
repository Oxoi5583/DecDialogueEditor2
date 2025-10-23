#pragma once

#include "glm/ext/vector_float2.hpp"

using namespace glm;

struct ShapeBase{
public:
    virtual bool is_point_intersect(vec2 p_pos){ return false; };
    virtual void set_position(vec2 p_pos){};
    virtual vec2 get_position() const{ return vec2(); };
};