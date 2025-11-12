#include "struct/shape/Circle.h"

#include "circle.h"
#include <utility>
#include <cmath>

Circle::Circle() : m_position(0.0f, 0.0f), m_radius(0.0) {}

Circle::Circle(const vec2& p_position, const vec2& p_size)
    : m_position(p_position), m_radius(p_size.x) {}

Circle::~Circle() = default;

Circle::Circle(const Circle& other)
    : m_position(other.m_position), m_radius(other.m_radius) {}

Circle::Circle(Circle&& other) noexcept
    : m_position(std::move(other.m_position)), m_radius(other.m_radius) {
    other.m_radius = 0.0;
}

Circle& Circle::operator=(const Circle& other) {
    if (this != &other) {
        m_position = other.m_position;
        m_radius = other.m_radius;
    }
    return *this;
}

Circle& Circle::operator=(Circle&& other) noexcept {
    if (this != &other) {
        m_position = std::move(other.m_position);
        m_radius = other.m_radius;
        other.m_radius = 0.0;
    }
    return *this;
}

void Circle::set_position(vec2 p_position) {
    m_position = p_position;
}

void Circle::set_radius(double p_radius) {
    m_radius = p_radius;
}

vec2 Circle::get_position() const {
    return m_position;
}
vec2 Circle::get_size() const {
    return vec2(m_radius * 2.0f, m_radius * 2.0f);
}


double Circle::get_radius() const {
    return m_radius;
}

bool Circle::is_point_intersect(vec2 p_pos) {
    vec2 diff = p_pos - m_position;
    double dist_sq = pow(diff.x, 2) + pow(diff.y, 2);
    return dist_sq <= pow(m_radius, 2);
}

