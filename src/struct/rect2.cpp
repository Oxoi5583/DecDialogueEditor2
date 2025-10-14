#include "struct/rect2.h"
#include "glm/ext/vector_float2.hpp"
#include <vector>

Rect2::Rect2() = default;

Rect2::Rect2(const vec2& p_position, const vec2& p_size)
    : m_position(p_position), m_size(p_size) {}
Rect2::~Rect2() = default;
Rect2::Rect2(const Rect2& other)
    : m_position(other.m_position), m_size(other.m_size) {}
Rect2::Rect2(Rect2&& other) noexcept
    : m_position(std::move(other.m_position)),
      m_size(std::move(other.m_size)) {}
Rect2& Rect2::operator=(const Rect2& other) {
    if (this != &other) {
        m_position = other.m_position;
        m_size = other.m_size;
    }
    return *this;
}
Rect2& Rect2::operator=(Rect2&& other) noexcept {
    if (this != &other) {
        m_position = std::move(other.m_position);
        m_size = std::move(other.m_size);
    }
    return *this;
}


void Rect2::set_size(vec2 p_size){
    m_size = p_size;
}
void Rect2::set_position(vec2 p_position){
    m_position = p_position;
}
void Rect2::set_center(vec2 p_center){
    m_position = p_center;
}
vec2 Rect2::get_size() const{
    return m_size;
}
vec2 Rect2::get_position() const{
    return m_position;
}
vec2 Rect2::get_left_top() const{
    return m_position + vec2(-m_size.x ,-m_size.y) / 2.0f;
}
vec2 Rect2::get_left_down() const{
    return m_position + vec2(-m_size.x ,m_size.y) / 2.0f;
}
vec2 Rect2::get_right_top() const{
    return m_position + vec2(m_size.x ,-m_size.y) / 2.0f;
}
vec2 Rect2::get_right_down() const{
    return m_position + vec2(m_size.x ,m_size.y) / 2.0f;
}
vec2 Rect2::get_center() const{
    return m_position;
}
std::vector<vec2> Rect2::get_points() const{
    std::vector<vec2> ret;
    ret.resize(4);
    
    vec2 left_top = get_left_top();
    vec2 left_down = get_left_down();
    vec2 right_top = get_right_top();
    vec2 right_down = get_right_down();

    ret.push_back(left_top);
    ret.push_back(right_top);
    ret.push_back(right_down);
    ret.push_back(left_down);

    return ret;
}


