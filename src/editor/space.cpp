#include "editor/space.h"
#include "DecToolsBox/debug/messenger.h"
#include "graph/camera.h"
#include "struct/shape/rect2.h"
#include <algorithm>
#include <array>
#include <queue>

EditorSpace::EditorSpace() = default;

EditorSpace::~EditorSpace() = default;

EditorSpace::EditorSpace(SplitType p_type, vec2 p_pos, vec2 p_size)
    : Rect2(p_pos, p_size), m_type(p_type) {}

EditorSpace::EditorSpace(SplitType p_type, vec2 p_pos, vec2 p_size, double p_proportion)
    : Rect2(p_pos, p_size), m_type(p_type) {
    split(p_proportion);
}

EditorSpace::EditorSpace(const EditorSpace& other)
    : Rect2(other)
    , m_split(other.m_split)
    , m_type(other.m_type)
    , m_spaces(other.m_spaces) {}

EditorSpace& EditorSpace::operator=(const EditorSpace& other) {
    if (this != &other) {
        Rect2::operator=(other);
        m_split = other.m_split;
        m_type = other.m_type;
        m_spaces = other.m_spaces;
    }
    return *this;
}

EditorSpace::EditorSpace(EditorSpace&& other) noexcept
    : Rect2(std::move(other))
    , m_split(other.m_split)
    , m_type(other.m_type)
    , m_spaces(std::move(other.m_spaces)) {
    other.m_split = 0.0;
}

EditorSpace& EditorSpace::operator=(EditorSpace&& other) noexcept {
    if (this != &other) {
        Rect2::operator=(std::move(other));
        m_split = other.m_split;
        m_type = other.m_type;
        m_spaces = std::move(other.m_spaces);
        other.m_split = 0.0;
    }
    return *this;
}

void EditorSpace::unsplit(){
    m_split = 0.0f;
    m_spaces.clear();
}

void EditorSpace::split(double m_proportion){
    m_split = m_proportion;

    if(m_spaces.size() == 0){
        m_spaces.push_back(EditorSpace());
        m_spaces.push_back(EditorSpace());
    }

    refresh_children();
}

void EditorSpace::set_type(SplitType p_type){
    m_type = p_type;
    refresh_children();
}

Rect2 EditorSpace::to_screen(){
    Rect2 ret = *this;

    vec2 pos = ret.get_position() - GraphCamera::Ref()->get_target();
    ret.set_position(pos);

    return ret;
}

EditorSpace::Children EditorSpace::get_children(){
    Children ret;

    if(!m_spaces.empty()){
        ret.first = &m_spaces[0];
        ret.second = &m_spaces[1];
    }

    return  ret;
}

std::vector<EditorSpace> EditorSpace::get_spaces() const{
    if(m_spaces.empty()){
        return {*this};
    }

    std::vector<EditorSpace> ret;
    
    for(EditorSpace s : m_spaces){
        auto s_cd = s.get_spaces();
        if(s_cd.size() == 1){
            ret.push_back(s_cd[0]);
            continue;
        }

        for(EditorSpace s2 : s_cd){
            ret.push_back(s2);
        }
    }

    return ret;
}

std::array<Rect2,2> EditorSpace::get_rects() const{
    switch (this->m_type) {
        case SplitType::VERTICLE:
            return m_get_rects_verticle();
            break;
        case SplitType::HORIZONTAL:
            return m_get_rects_horizontal();
            break;
    }
}

std::array<Rect2,2> EditorSpace::m_get_rects_verticle() const{
    vec2 left_top = this->get_left_top();
    vec2 size = this->get_size();
    
    vec2 r1_size = vec2(size.x, size.y * m_split);
    vec2 r1_left_top = left_top;
    vec2 r1_center = r1_left_top + (r1_size / 2.0f);

    vec2 r2_size = vec2(size.x, size.y - r1_size.y);
    vec2 r2_left_top = r1_left_top + vec2(0, r1_size.y);
    vec2 r2_center = r2_left_top + (r2_size / 2.0f);

    return {Rect2(r1_center, r1_size), Rect2(r2_center, r2_size)};
}
std::array<Rect2,2> EditorSpace::m_get_rects_horizontal() const{
    vec2 left_top = this->get_left_top();
    vec2 size = this->get_size();

    vec2 r1_size = vec2(size.x * m_split, size.y);
    vec2 r1_left_top = left_top;
    vec2 r1_center = r1_left_top + (r1_size / 2.0f);

    vec2 r2_size = vec2(size.x - r1_size.x, size.y);
    vec2 r2_left_top = r1_left_top + vec2(r1_size.x, 0.0f);
    vec2 r2_center = r2_left_top + (r2_size / 2.0f);

    return {Rect2(r1_center, r1_size), Rect2(r2_center, r2_size)};
}

double EditorSpace::get_proportion(vec2 p_pos) const{
    switch (this->m_type) {
        case SplitType::VERTICLE:
            return m_get_proportion_verticle(p_pos);
            break;
        case SplitType::HORIZONTAL:
            return m_get_proportion_horizontal(p_pos);
            break;
    }
}

double EditorSpace::m_get_proportion_verticle(vec2 p_pos) const{
    double pos_y = p_pos.y;

    double start = this->get_left_top().y;
    double end = this->get_left_down().y;

    double clamp_pos = std::clamp(pos_y, start, end);
    
    double dist = clamp_pos - start;
    double total_dist = end - start;

    return dist / total_dist;
}

double EditorSpace::m_get_proportion_horizontal(vec2 p_pos) const{
    double pos_x = p_pos.x;

    double start = this->get_left_top().x;
    double end = this->get_right_top().x;

    double clamp_pos = std::clamp(pos_x, start, end);
    
    double dist = clamp_pos - start;
    double total_dist = end - start;

    return dist / total_dist;
}

void EditorSpace::refresh_children(){
    if(m_spaces.size() == 0){
        return;
    }

    std::array<Rect2, 2> rects = this->get_rects();

    m_spaces[0].set_center(rects[0].get_center());
    m_spaces[0].set_size(rects[0].get_size());
    m_spaces[1].set_center(rects[1].get_center());
    m_spaces[1].set_size(rects[1].get_size());
}
