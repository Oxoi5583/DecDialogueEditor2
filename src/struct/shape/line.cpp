#include "struct/shape/line.h"
#include "DecToolsBox/debug/messenger.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"
#include "ext/debug/messenger_ext.h"

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

vec2 Line::get_from() const{
    return m_from_pos;
}
vec2 Line::get_to() const{
    return m_to_pos;
}
void Line::set_from(vec2& p_pos){
    m_from_pos = p_pos;
}
void Line::set_to(vec2& p_pos){
    m_to_pos = p_pos;
}

bool Line::is_point_intersect(vec2 p_pos){
    return m_cross_product(p_pos);
}
void Line::set_position(vec2 p_pos){
    vec2 old_pos = get_position();
    vec2 new_pos = p_pos;
    vec2 delta = new_pos - old_pos;

    m_from_pos += delta;
    m_to_pos += delta;
}
vec2 Line::get_position() const{
    vec2 min = glm::min(m_from_pos, m_to_pos);
    vec2 size = this->get_size();

    return min + size;
}
vec2 Line::get_size() const{
    vec2 max = glm::max(m_from_pos, m_to_pos);
    vec2 min = glm::min(m_from_pos, m_to_pos);

    return max - min;
}

std::vector<vec2> Line::get_points() const{
    vec2 dir = glm::normalize(m_to_pos - m_from_pos);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 left_model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f,0.0f,1.0f));
    glm::mat4 right_model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f,0.0f,1.0f));
    
    vec2 left = vec4(dir,0.0f,1.0f) * left_model;
    vec2 right = vec4(dir,0.0f,1.0f) * right_model;

    double half_width = m_width / 2.0f;

    vec2 left_fm = m_from_pos + (left * vec2(half_width, half_width));
    vec2 right_fm = m_from_pos + (right * vec2(half_width, half_width));
    vec2 left_to = m_to_pos + (left * vec2(half_width, half_width));
    vec2 right_to = m_to_pos + (right * vec2(half_width, half_width));
    
    return {left_to, right_to, right_fm, left_fm};
}


bool Line::m_cross_product(vec2 p_pos){
    vec2 lt_pos = this->m_get_left_from();
    vec2 ld_pos = this->m_get_left_to();
    vec2 rd_pos = this->m_get_right_to();
    vec2 rt_pos = this->m_get_right_from();

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
    DEBUG_MSG("cross_ret[0] * cross_ret[1] : " << cross_ret[0] << " * " << cross_ret[1]);
    if(cross_ret[0] * cross_ret[1] < 0) return false;
    DEBUG_MSG(2);
    if(cross_ret[1] * cross_ret[2] < 0) return false;
    DEBUG_MSG(3);
    if(cross_ret[2] * cross_ret[3] < 0) return false;
    DEBUG_MSG(4);
    if(cross_ret[3] * cross_ret[0] < 0) return false;

    DEBUG_MSG(5);
    return true;
}

bool Line::m_compare_xy(vec2 p_pos){
    vec2 left_top = m_get_left_from();
    vec2 right_down = m_get_right_from();

    if(p_pos.x < left_top.x || p_pos.x > right_down.x){
        return false;
    }
    if(p_pos.y < left_top.y || p_pos.y > right_down.y){
        return false;
    }
    
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
    vec2 rd = this->m_get_right_to();
    vec2 rt = this->m_get_right_from();

    vec2 rectPts[4] = { lt, ld, rd, rt };

    if (m_cross_product(p1) || m_cross_product(p2)){
        return true;
    }

    for (int i = 0; i < 4; i++) {
        vec2 a = rectPts[i];
        vec2 b = rectPts[(i + 1) % 4];

        if (m_segment_intersect(p1, p2, a, b)){
            return true;
        }
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
    vec2 dir = get_dir(m_from_pos, m_to_pos);
    vec2 normal = get_left_normal(dir);
    double half_width = m_width * 0.5;

    return m_from_pos + normal * (float)half_width;
}

vec2 Line::m_get_left_to() {
    vec2 dir = get_dir(m_from_pos, m_to_pos);
    vec2 normal = get_left_normal(dir);
    double half_width = m_width * 0.5;

    return m_to_pos + normal * (float)half_width;
}

vec2 Line::m_get_right_from() {
    vec2 dir = get_dir(m_from_pos, m_to_pos);
    vec2 normal = get_right_normal(dir);
    double half_width = m_width * 0.5;

    return m_from_pos + normal * (float)half_width;
}

vec2 Line::m_get_right_to() {
    vec2 dir = get_dir(m_from_pos, m_to_pos);
    vec2 normal = get_right_normal(dir);
    double half_width = m_width * 0.5;

    return m_to_pos + normal * (float)half_width;
}
double Line::get_width() const{
    return m_width;
}
void Line::set_width(double p_width){
    m_width = p_width;
}
bool Line::is_line_intersect(const Line& p_line){
    Line line_buf = p_line;
    double line_width = line_buf.get_width();
    
    if(line_width < 1.0f){
        line_width = 1.01f;
    }

    line_buf.set_width(line_width);

    Line this_buf = *this;
    double this_width = this_buf.get_width();
    
    if(this_width < 1.0f){
        this_width = 1.01f;
    }

    this_buf.set_width(this_width);

    std::vector<vec2> points = line_buf.get_points();
    if(this_buf.is_segment_intersect(points[0], points[1])) return true;
    if(this_buf.is_segment_intersect(points[1], points[2])) return true;
    if(this_buf.is_segment_intersect(points[2], points[3])) return true;
    if(this_buf.is_segment_intersect(points[3], points[0])) return true;
    return false;
}
