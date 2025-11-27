#pragma once

#include "glm/ext/vector_float2.hpp"
#include "struct/shape/base.h"
#include <vector>

using namespace glm;

struct Line : public ShapeBase {
private:
    vec2 m_from_pos;
    vec2 m_to_pos;

    double m_width;

    bool m_cross_product(vec2 p_pos);
    bool m_compare_xy(vec2 p_pos);
    bool m_segment_intersect(const vec2& p1, const vec2& p2,
                             const vec2& p3, const vec2& p4);

    vec2 m_get_left_from();
    vec2 m_get_left_to();
    vec2 m_get_right_from();
    vec2 m_get_right_to();

public:
    Line() = default;
    Line(vec2 p_fm, vec2 p_to, double p_width);
    Line(const Line& other);
    Line(Line&& other) noexcept;
    Line& operator=(const Line& other);
    Line& operator=(Line&& other) noexcept;

    ~Line() = default;

    vec2 get_from() const;
    vec2 get_to() const;
    double get_width() const;
    void set_from(vec2 p_pos);
    void set_to(vec2 p_pos);
    void set_width(double p_width);

    bool is_point_intersect(vec2 p_pos) override;
    bool is_segment_intersect(const vec2& p1, const vec2& p2);
    bool is_line_intersect(const Line& p_line);

    void set_position(vec2 p_pos) override;
    vec2 get_position() const override;
    vec2 get_size() const override;

    std::vector<vec2> get_points() const;
};
