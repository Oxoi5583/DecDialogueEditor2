#include "struct/shape/rect2.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "glm/common.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
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
void Rect2::set_left_top(vec2 p_position){
    m_position = p_position + (m_size / 2.0f);
}
void Rect2::set_center(vec2 p_center){
    m_position = p_center;
}
void Rect2::set_AABB(vec2 p_left_top, vec2 p_right_down){
    vec2 left_top = glm::min(p_left_top, p_right_down);
    vec2 right_down = glm::max(p_left_top, p_right_down);

    vec2 size = right_down - left_top;
    vec2 center = left_top + (size / 2.0f);
    set_size(size);
    set_center(center);
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

    ret[0] = left_top;
    ret[1] = right_top;
    ret[2] = right_down;
    ret[3] = left_down;

    return ret;
}


bool Rect2::m_cross_product(vec2 p_pos){
    vec2 lt_pos = this->get_left_top();
    vec2 ld_pos = this->get_left_down();
    vec2 rd_pos = this->get_right_down();
    vec2 rt_pos = this->get_right_top();

    vec3 rect_dir[4] = {
        vec3(ld_pos - lt_pos, 0.0f),
        vec3(rd_pos - ld_pos, 0.0f),
        vec3(rt_pos - rd_pos, 0.0f),
        vec3(lt_pos - rt_pos, 0.0f),
    };
    vec3 pos_dir[4] = {
        vec3(p_pos - lt_pos, 0.0f),
        vec3(p_pos - ld_pos, 0.0f),
        vec3(p_pos - rd_pos, 0.0f),
        vec3(p_pos - rt_pos, 0.0f),
    };
    float cross_ret[4] = {
        glm::cross(rect_dir[0], pos_dir[0]).z,
        glm::cross(rect_dir[1], pos_dir[1]).z,
        glm::cross(rect_dir[2], pos_dir[2]).z,
        glm::cross(rect_dir[3], pos_dir[3]).z,
    };

    if(cross_ret[0] * cross_ret[1] < 0) return false;
    if(cross_ret[1] * cross_ret[2] < 0) return false;
    if(cross_ret[2] * cross_ret[3] < 0) return false;
    if(cross_ret[3] * cross_ret[0] < 0) return false;

    return true;
}

bool Rect2::m_compare_xy(vec2 p_pos){
    vec2 left_top = get_left_top();
    vec2 right_down = get_right_down();

    if(p_pos.x < left_top.x || p_pos.x > right_down.x){
        return false;
    }
    if(p_pos.y < left_top.y || p_pos.y > right_down.y){
        return false;
    }
    
    return true;
}

bool Rect2::is_point_intersect(vec2 p_pos){
    return m_compare_xy(p_pos);
}

bool Rect2::is_rect_intersect(Rect2 p_rect){
    std::vector<vec2> points = p_rect.get_points();
    for(vec2 point : points){
        if(this->is_point_intersect(point)){
            return true;
        }
    }

    std::vector<vec2> rev_points = this->get_points();
    for(vec2 point : rev_points){
        if(p_rect.is_point_intersect(point)){
            return true;
        }
    }

    return false;
}


void Rect2::move_left_top(vec2 p_pos){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = p_pos;
    vec2 new_rd = old_rd;

    this->set_AABB(new_lt, new_rd);
}
void Rect2::move_left_down(vec2 p_pos){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = { p_pos.x, old_lt.y };
    vec2 new_rd = { old_rd.x, p_pos.y };

    this->set_AABB(new_lt, new_rd);
}
void Rect2::move_right_top(vec2 p_pos){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = { old_lt.x, p_pos.y };
    vec2 new_rd = { p_pos.x, old_lt.y };

    this->set_AABB(new_lt, new_rd);
}
void Rect2::move_right_down(vec2 p_pos){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = {old_lt.x,old_lt.y};
    vec2 new_rd = {p_pos.x, p_pos.y};

    this->set_AABB(new_lt, new_rd);
}
void Rect2::move_left(double p_x){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = { p_x, old_lt.y };
    vec2 new_rd = old_rd;

    this->set_AABB(new_lt, new_rd);

}
void Rect2::move_right(double p_x){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = old_lt;
    vec2 new_rd = { p_x, old_rd.y };

    this->set_AABB(new_lt, new_rd);
}
void Rect2::move_top(double p_y){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = { old_lt.x, p_y };
    vec2 new_rd = old_rd;

    this->set_AABB(new_lt, new_rd);
}
void Rect2::move_down(double p_y){
    vec2 old_lt = this->get_left_top();
    vec2 old_rd = this->get_right_down();

    vec2 new_lt = old_lt;
    vec2 new_rd = { old_rd.x, p_y };

    this->set_AABB(new_lt, new_rd);
}