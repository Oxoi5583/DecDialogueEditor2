#include "struct/shape/line.h"
#include "DecToolsBox/debug/messenger.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include "ext/debug/messenger_ext.h"
#include <cstddef>

Line::Line(vec2 p_fm, vec2 p_to, double p_width)
    : m_from_pos(p_fm),
      m_to_pos(p_to),
      m_width(p_width)
{}

Line::Line(const Line& other)
    : ShapeBase(other),
      m_from_pos(other.m_from_pos),
      m_to_pos(other.m_to_pos),
      m_width(other.m_width)
{}

Line::Line(Line&& other) noexcept
    : ShapeBase(std::move(other)),
      m_from_pos(other.m_from_pos),
      m_to_pos(other.m_to_pos),
      m_width(other.m_width)
{
    other.m_from_pos = vec2(0.0f);
    other.m_to_pos = vec2(0.0f);
    other.m_width = 0.0;
}

Line& Line::operator=(const Line& other)
{
    if (this != &other) {
        ShapeBase::operator=(other);
        m_from_pos = other.m_from_pos;
        m_to_pos = other.m_to_pos;
        m_width = other.m_width;
    }
    return *this;
}

Line& Line::operator=(Line&& other) noexcept
{
    if (this != &other) {
        ShapeBase::operator=(std::move(other));
        m_from_pos = other.m_from_pos;
        m_to_pos = other.m_to_pos;
        m_width = other.m_width;

        other.m_from_pos = vec2(0.0f);
        other.m_to_pos = vec2(0.0f);
        other.m_width = 0.0;
    }
    return *this;
}

vec2 Line::get_from() const { return m_from_pos; }
vec2 Line::get_to()   const { return m_to_pos; }

void Line::set_from(vec2 p_pos){
    m_from_pos = p_pos;
    m_get_left_from();
}

void Line::set_to(vec2 p_pos){
    m_to_pos = p_pos;
}

bool Line::is_point_intersect(vec2 p_pos){
    return m_cross_product(p_pos);
}

void Line::set_position(vec2 p_pos){
    vec2 old_pos = get_position();
    vec2 delta = p_pos - old_pos;
    m_from_pos += delta;
    m_to_pos += delta;
}

vec2 Line::get_position() const{
    vec2 minv = glm::min(get_from(), get_to());
    vec2 size = this->get_size();
    return minv + size;
}

vec2 Line::get_size() const{
    vec2 maxv = glm::max(get_from(), get_to());
    vec2 minv = glm::min(get_from(), get_to());
    return maxv - minv;
}

std::vector<vec2> Line::get_points() const{
    vec2 dir = glm::normalize(get_to() - get_from());

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 left_model  = glm::rotate(model, glm::radians(90.0f),  glm::vec3(0,0,1));
    glm::mat4 right_model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0,0,1));

    vec2 left  = vec4(dir,0.0f,1.0f) * left_model;
    vec2 right = vec4(dir,0.0f,1.0f) * right_model;

    double half_width = m_width * 0.5;

    vec2 left_fm  = get_from() + left  * (float)half_width;
    vec2 right_fm = get_from() + right * (float)half_width;
    vec2 left_to  = get_to()   + left  * (float)half_width;
    vec2 right_to = get_to()   + right * (float)half_width;
    
    return {left_to, right_to, right_fm, left_fm};
}

bool Line::m_cross_product(vec2 p_pos){
    if(m_from_pos == m_to_pos){
        if(m_from_pos == p_pos){
            return true;
        }
        
        return false;
    }


    vec2 lt_pos = m_get_left_from();
    vec2 ld_pos = m_get_left_to();
    vec2 rd_pos = m_get_right_to();
    vec2 rt_pos = m_get_right_from();

    vec3 rect_dir[4] = {
        vec3(glm::normalize(ld_pos - lt_pos), 0.0f),
        vec3(glm::normalize(rd_pos - ld_pos), 0.0f),
        vec3(glm::normalize(rt_pos - rd_pos), 0.0f),
        vec3(glm::normalize(lt_pos - rt_pos), 0.0f),
    };
    vec3 pos_dir[4] = {
        vec3(glm::normalize(p_pos - lt_pos), 0.0f),
        vec3(glm::normalize(p_pos - ld_pos), 0.0f),
        vec3(glm::normalize(p_pos - rd_pos), 0.0f),
        vec3(glm::normalize(p_pos - rt_pos), 0.0f),
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

bool Line::m_compare_xy(vec2 p_pos){
    vec2 left_top  = m_get_left_from();
    vec2 right_down = m_get_right_from();

    if(p_pos.x < left_top.x || p_pos.x > right_down.x) return false;
    if(p_pos.y < left_top.y || p_pos.y > right_down.y) return false;
    
    return true;
}

static float cross2D(const vec2& a, const vec2& b) {
    return a.x * b.y - a.y * b.x;
}

bool Line::m_segment_intersect(const vec2& p1, const vec2& p2,
                               const vec2& p3, const vec2& p4){
    vec2 r = p2 - p1;
    vec2 s = p4 - p3;

    float rxs = cross2D(r, s);
    float qpxr = cross2D((p3 - p1), r);

    if (fabs(rxs) < 1e-6) return false;

    float t = cross2D((p3 - p1), s) / rxs;
    float u = qpxr / rxs;

    return (t >= 0.0f && t <= 1.0f &&
            u >= 0.0f && u <= 1.0f);
}

bool Line::is_segment_intersect(const vec2& p1, const vec2& p2){
    vec2 lt = m_get_left_from();
    vec2 ld = m_get_left_to();
    vec2 rd = m_get_right_to();
    vec2 rt = m_get_right_from();

    vec2 rectPts[4] = { lt, ld, rd, rt };

    if (m_cross_product(p1) || m_cross_product(p2)) return true;

    for (int i = 0; i < 4; i++) {
        vec2 a = rectPts[i];
        vec2 b = rectPts[(i + 1) % 4];
        if (m_segment_intersect(p1, p2, a, b)) return true;
    }

    return false;
}

inline vec2 get_dir(const vec2& from, const vec2& to) {
    return glm::normalize(to - from);
}

inline vec2 get_left_normal(const vec2& dir) {
    return vec2(-dir.y, dir.x);
}

inline vec2 get_right_normal(const vec2& dir) {
    return vec2(dir.y, -dir.x);
}

vec2 Line::m_get_left_from() {
    vec2 dir = get_dir(get_from(), get_to());
    vec2 normal = get_left_normal(dir);
    return get_from() + normal * (float)(m_width * 0.5);
}

vec2 Line::m_get_left_to() {
    vec2 dir = get_dir(get_from(), get_to());
    vec2 normal = get_left_normal(dir);
    return get_to() + normal * (float)(m_width * 0.5);
}

vec2 Line::m_get_right_from() {
    vec2 dir = get_dir(get_from(), get_to());
    vec2 normal = get_right_normal(dir);
    return get_from() + normal * (float)(m_width * 0.5);
}

vec2 Line::m_get_right_to() {
    vec2 dir = get_dir(get_from(), get_to());
    vec2 normal = get_right_normal(dir);
    return get_to() + normal * (float)(m_width * 0.5);
}

double Line::get_width() const { return m_width; }
void Line::set_width(double p_width) { m_width = p_width; }

bool Line::is_line_intersect(const Line& p_line){
    Line line_buf = p_line;
    double line_width = line_buf.get_width();
    
    if(line_width < 1.0f) line_width = 1.01f;
    line_buf.set_width(line_width);

    Line this_buf = *this;
    double this_width = this_buf.get_width();
    
    if(this_width < 1.0f) this_width = 1.01f;
    this_buf.set_width(this_width);

    std::vector<vec2> points = line_buf.get_points();
    if(this_buf.is_segment_intersect(points[0], points[1])) return true;
    if(this_buf.is_segment_intersect(points[1], points[2])) return true;
    if(this_buf.is_segment_intersect(points[2], points[3])) return true;
    if(this_buf.is_segment_intersect(points[3], points[0])) return true;

    return false;
}
