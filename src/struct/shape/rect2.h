#pragma once

#include "glm/ext/vector_float2.hpp"
#include "struct/shape/base.h"
#include <vector>

using namespace glm;

struct Rect2 : public ShapeBase {
private:
    vec2 m_position;
    vec2 m_size;

    bool m_cross_product(vec2 p_pos);
    bool m_compare_xy(vec2 p_pos);
public:
    Rect2();
    Rect2(const vec2& p_position, const vec2& p_size);
    ~Rect2();

    Rect2(const Rect2& other);
    Rect2(Rect2&& other) noexcept;
    Rect2& operator=(const Rect2& other);
    Rect2& operator=(Rect2&& other) noexcept;

    void set_size(vec2 p_size);
    void set_position(vec2 p_position);
    void set_center(vec2 p_center);

    vec2 get_size() const;
    vec2 get_position() const;
    vec2 get_left_top() const;
    vec2 get_left_down() const;
    vec2 get_right_top() const;
    vec2 get_right_down() const;
    vec2 get_center() const;
    std::vector<vec2> get_points() const;

    bool is_point_intersect(vec2 p_pos) override;
};