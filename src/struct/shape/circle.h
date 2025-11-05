#pragma once

#include "glm/ext/vector_float2.hpp"
#include "struct/shape/base.h"
#include <vector>

using namespace glm;

struct Circle : public ShapeBase {
private:
    vec2 m_position;
    double m_radius;

public:
    Circle();
    Circle(const vec2& p_position, const vec2& p_size);
    ~Circle();

    Circle(const Circle& other);
    Circle(Circle&& other) noexcept;
    Circle& operator=(const Circle& other);
    Circle& operator=(Circle&& other) noexcept;

    void set_position(vec2 p_position) override;
    void set_radius(double p_radius);
    
    vec2 get_position() const override;
    double get_radius() const;

    bool is_point_intersect(vec2 p_pos) override;
};