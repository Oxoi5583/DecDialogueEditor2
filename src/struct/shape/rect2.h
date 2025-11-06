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

    void set_position(vec2 p_position) override;
    void set_left_top(vec2 p_position);
    void set_size(vec2 p_size);
    void set_center(vec2 p_center);
    void set_AABB(vec2 p_left_top, vec2 p_right_down);
    
    vec2 get_size() const;
    vec2 get_position() const override;
    vec2 get_left_top() const;
    vec2 get_left_down() const;
    vec2 get_right_top() const;
    vec2 get_right_down() const;
    vec2 get_center() const;
    std::vector<vec2> get_points() const;


    bool is_point_intersect(vec2 p_pos) override;
    bool is_rect_intersect(Rect2 p_rect);

    
    void move_left_top(vec2 p_pos);
    void move_left_down(vec2 p_pos);
    void move_right_top(vec2 p_pos);
    void move_right_down(vec2 p_pos);
    void move_left(double p_x);
    void move_right(double p_x);
    void move_top(double p_y);
    void move_down(double p_y);
};