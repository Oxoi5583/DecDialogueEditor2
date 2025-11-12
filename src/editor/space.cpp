#include "editor/space.h"
#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "graph/camera.h"
#include "graph/viewport.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "struct/shape/rect2.h"
#include "ext/debug/messenger_ext.h"
#include <algorithm>
#include <array>
#include <limits>
#include <queue>
#include <utility>

void EditorSpace::apply_magnet(SplitMagnet& p_magnet, double& p_split){
    double start;
    double end;
    double value;

    switch (p_magnet.type) {
        case SplitMagnet::Type::PROPORTION:{
            start = p_magnet.magnet - p_magnet.range.first;
            end = p_magnet.magnet + p_magnet.range.second;
            value = p_magnet.magnet;
            break;
        }
        case SplitMagnet::Type::VALUE:{
            start = m_value_to_proportion(p_magnet.magnet - p_magnet.range.first);
            end = m_value_to_proportion(p_magnet.magnet + p_magnet.range.second);
            value = m_value_to_proportion(p_magnet.magnet);
            break;
        }
    }

    Range r = {start, end};
    if(r.is_in_range(p_split)){
        p_split = value;
    }
}

// ===================== SplitLimit =====================
EditorSpace::SplitLimit::SplitLimit(const SplitLimit& other)
    : min(other.min),
      max(other.max),
      min_type(other.min_type),
      max_type(other.max_type),
      m_is_enabled(other.m_is_enabled) {}

EditorSpace::SplitLimit& EditorSpace::SplitLimit::operator=(const SplitLimit& other) {
    if (this == &other)
        return *this;
    min = other.min;
    max = other.max;
    min_type = other.min_type;
    max_type = other.max_type;
    m_is_enabled = other.m_is_enabled;
    return *this;
}

EditorSpace::SplitLimit::SplitLimit(SplitLimit&& other) noexcept
    : min(other.min),
      max(other.max),
      min_type(other.min_type),
      max_type(other.max_type),
      m_is_enabled(other.m_is_enabled) {
    // no special cleanup required
}

EditorSpace::SplitLimit& EditorSpace::SplitLimit::operator=(SplitLimit&& other) noexcept {
    if (this == &other)
        return *this;
    min = other.min;
    max = other.max;
    min_type = other.min_type;
    max_type = other.max_type;
    m_is_enabled = other.m_is_enabled;
    return *this;
}

// ===================== SplitFixed =====================
EditorSpace::SplitFixed::SplitFixed(const SplitFixed& other)
    : value(other.value),
      type(other.type),
      m_is_enabled(other.m_is_enabled) {}

EditorSpace::SplitFixed& EditorSpace::SplitFixed::operator=(const SplitFixed& other) {
    if (this == &other)
        return *this;
    value = other.value;
    type = other.type;
    m_is_enabled = other.m_is_enabled;
    return *this;
}

EditorSpace::SplitFixed::SplitFixed(SplitFixed&& other) noexcept
    : value(other.value),
      type(other.type),
      m_is_enabled(other.m_is_enabled) {}

EditorSpace::SplitFixed& EditorSpace::SplitFixed::operator=(SplitFixed&& other) noexcept {
    if (this == &other)
        return *this;
    value = other.value;
    type = other.type;
    m_is_enabled = other.m_is_enabled;
    return *this;
}

// ===================== SplitResizer =====================
EditorSpace::SplitResizer::SplitResizer(const SplitResizer& other)
    : resizer_size(other.resizer_size),
      m_is_enabled(other.m_is_enabled),
      state(other.state) {}

EditorSpace::SplitResizer& EditorSpace::SplitResizer::operator=(const SplitResizer& other) {
    if (this == &other)
        return *this;
    resizer_size = other.resizer_size;
    m_is_enabled = other.m_is_enabled;
    state = other.state;
    return *this;
}

EditorSpace::SplitResizer::SplitResizer(SplitResizer&& other) noexcept
    : resizer_size(other.resizer_size),
      m_is_enabled(other.m_is_enabled),
      state(other.state) {}

EditorSpace::SplitResizer& EditorSpace::SplitResizer::operator=(SplitResizer&& other) noexcept {
    if (this == &other)
        return *this;
    resizer_size = other.resizer_size;
    m_is_enabled = other.m_is_enabled;
    state = other.state;
    return *this;
}


// --- Copy Constructor ---
EditorSpace::EditorSpace(const EditorSpace& other)
    : Rect2(other),
      from(other.from),
      split_limit(other.split_limit),
      split_fixed(other.split_fixed),
      split_resizer(other.split_resizer),
      split_magnets(other.split_magnets),
      m_split(other.m_split),
      m_type(other.m_type),
      m_spaces(other.m_spaces),
      m_parent(other.m_parent),
      m_split_value_buffer(other.m_split_value_buffer) {}

// --- Copy Assignment ---
EditorSpace& EditorSpace::operator=(const EditorSpace& other) {
    if (this == &other)
        return *this;

    Rect2::operator=(other);
    from = other.from;
    split_limit = other.split_limit;
    split_fixed = other.split_fixed;
    split_resizer = other.split_resizer;
    split_magnets = other.split_magnets;
    m_split = other.m_split;
    m_type = other.m_type;
    m_spaces = other.m_spaces;
    m_parent = other.m_parent;
    m_split_value_buffer = other.m_split_value_buffer;
    return *this;
}

// --- Move Constructor ---
EditorSpace::EditorSpace(EditorSpace&& other) noexcept
    : Rect2(std::move(other)),
      from(std::exchange(other.from, From::START)),
      split_limit(std::move(other.split_limit)),
      split_fixed(std::move(other.split_fixed)),
      split_resizer(std::move(other.split_resizer)),
      split_magnets(std::move(other.split_magnets)),
      m_split(std::exchange(other.m_split, 0.0)),
      m_type(std::exchange(other.m_type, SplitType::VERTICLE)),
      m_spaces(std::move(other.m_spaces)),
      m_parent(std::exchange(other.m_parent, nullptr)),
      m_split_value_buffer(std::exchange(other.m_split_value_buffer, 0.0)) {}

// --- Move Assignment ---
EditorSpace& EditorSpace::operator=(EditorSpace&& other) noexcept {
    if (this == &other)
        return *this;

    Rect2::operator=(std::move(other));
    from = std::exchange(other.from, From::START);
    split_limit = std::move(other.split_limit);
    split_fixed = std::move(other.split_fixed);
    split_resizer = std::move(other.split_resizer);
    split_magnets = std::move(other.split_magnets);
    m_split = std::exchange(other.m_split, 0.0);
    m_type = std::exchange(other.m_type, SplitType::VERTICLE);
    m_spaces = std::move(other.m_spaces);
    m_parent = std::exchange(other.m_parent, nullptr);
    m_split_value_buffer = std::exchange(other.m_split_value_buffer, 0.0);
    return *this;
}


void EditorSpace::SplitLimit::enable(){
    m_is_enabled = true;
}
void EditorSpace::SplitLimit::disable(){
    m_is_enabled = false;
}
bool EditorSpace::SplitLimit::is_enabled(){
    return m_is_enabled;
}
void EditorSpace::SplitFixed::enable(){
    m_is_enabled = true;
}
void EditorSpace::SplitFixed::disable(){
    m_is_enabled = false;
}
bool EditorSpace::SplitFixed::is_enabled(){
    return m_is_enabled;
}
void EditorSpace::SplitResizer::enable(){
    m_is_enabled = true;
}
void EditorSpace::SplitResizer::disable(){
    m_is_enabled = false;
}
bool EditorSpace::SplitResizer::is_enabled(){
    return m_is_enabled;
}

Rect2 EditorSpace::get_resizer_area(){
    if(m_spaces.empty()){
        return {{std::numeric_limits<double>::max()
        ,std::numeric_limits<double>::max()}
        ,{0.0f,0.0f}};
    }

    switch (m_type) {
        case SplitType::VERTICLE:{
            std::array<Rect2,2> rects = m_get_rects_verticle();
            switch (from) {
                case From::START:{
                    Rect2 first_rect = rects[0];
                    vec2 get_start_pos = first_rect.get_left_down();
                    vec2 size = {first_rect.get_right_down().x - get_start_pos.x, split_resizer.resizer_size};
                    return {get_start_pos + (size / 2.0f), size};
                }
                case From::END:{
                    Rect2 first_rect = rects[1];
                    vec2 get_start_pos = first_rect.get_right_top();
                    vec2 size = {get_start_pos.x - first_rect.get_left_top().x, split_resizer.resizer_size};
                    return {get_start_pos - (size / 2.0f), size};
                }
            }
        }
        case SplitType::HORIZONTAL:{
            std::array<Rect2,2> rects = m_get_rects_horizontal();
            switch (from) {
                case From::START:{
                    Rect2 first_rect = rects[0];
                    vec2 get_start_pos = first_rect.get_right_top();
                    vec2 size = {split_resizer.resizer_size, first_rect.get_right_down().y - get_start_pos.y};
                    return {get_start_pos + (size / 2.0f), size};
                }
                case From::END:{
                    Rect2 first_rect = rects[1];
                    vec2 get_start_pos = first_rect.get_left_down();
                    vec2 size = {split_resizer.resizer_size, get_start_pos.y - first_rect.get_left_top().y};
                    return {get_start_pos - (size / 2.0f), size};
                }
            }
        }
    }
}
EditorSpace::SplitResizer::State EditorSpace::SplitResizer::m_go_to_state(State p_state){
    switch (p_state) {
        case State::IDLE:{
            m_trans_state_event_to_IDLE();
            break;
        }
        case State::HOVER:{
            m_trans_state_event_to_HOVER();
            break;
        }
        case State::DRAGGING:{
            m_trans_state_event_to_DRAGGING();
            break;
        }
    }
    state = p_state;
    return p_state;
}
void EditorSpace::SplitResizer::m_emit_event(){
    switch (state) {
        case State::IDLE:{
            m_state_event_in_IDLE();
            break;
        }
        case State::HOVER:{
            m_state_event_in_HOVER();
            break;
        }
        case State::DRAGGING:{
            m_state_event_in_DRAGGING();
            break;
        }
    }
}

void EditorSpace::update_resizer(){
    if(!split_resizer.is_enabled()){
        return;
    }
    switch (split_resizer.state) {
        case SplitResizer::State::IDLE:{
            vec2 mouse_pos = MouseServer::Ref()->get_mouse_screen_position_center();
            
            bool is_mouse_in_resizer = get_resizer_area().is_point_intersect(mouse_pos);
            bool is_other_resizer_hovering = EventServer::Ref()->has<EventEditorSpaceResizerHover>();

            if(is_mouse_in_resizer && !is_other_resizer_hovering) split_resizer.m_go_to_state(SplitResizer::State::HOVER);
            break;
        }
        case SplitResizer::State::HOVER:{
            vec2 mouse_pos = MouseServer::Ref()->get_mouse_screen_position_center();

            bool is_mouse_in_resizer = get_resizer_area().is_point_intersect(mouse_pos);
            bool is_mouse_left_just_clicked = MouseServer::Ref()->is_just_clicked();
            bool is_other_resizer_dragging = EventServer::Ref()->has<EventEditorSpaceResizerDragging>();

            if(!is_mouse_in_resizer) split_resizer.m_go_to_state(SplitResizer::State::IDLE);
            if(is_mouse_in_resizer && is_mouse_left_just_clicked && !is_other_resizer_dragging) split_resizer.m_go_to_state(SplitResizer::State::DRAGGING);
            break;
        }
        case SplitResizer::State::DRAGGING:{
            vec2 mouse_pos = MouseServer::Ref()->get_mouse_screen_position_center();
            double proportion = this->get_proportion(mouse_pos);
            this->split(proportion);

            bool is_mouse_left_just_released = MouseServer::Ref()->is_just_released();

            if(is_mouse_left_just_released) split_resizer.m_go_to_state(SplitResizer::State::IDLE);
            break;
        }
    }

    split_resizer.m_emit_event();
}
bool EditorSpace::is_resizing(){
    return split_resizer.state == SplitResizer::State::DRAGGING;
}

void EditorSpace::SplitResizer::m_trans_state_event_to_IDLE(){  }
void EditorSpace::SplitResizer::m_trans_state_event_to_HOVER(){  }
void EditorSpace::SplitResizer::m_trans_state_event_to_DRAGGING(){  }
void EditorSpace::SplitResizer::m_state_event_in_IDLE(){  }
void EditorSpace::SplitResizer::m_state_event_in_HOVER(){ 
    EventEditorSpaceResizerHover event1;
    EventServer::Ref()->emit(event1);
    EventMouseHoverObj event2;
    event2.is_pointer_cursor = true;
    EventServer::Ref()->emit(event2);
}
void EditorSpace::SplitResizer::m_state_event_in_DRAGGING(){
    EventEditorSpaceResizerHover event1;
    EventServer::Ref()->emit(event1);
    EventMouseHoverObj event2;
    event2.is_pointer_cursor = true;
    EventServer::Ref()->emit(event2);
}



EditorSpace::EditorSpace() = default;

EditorSpace::~EditorSpace() = default;

EditorSpace::EditorSpace(SplitType p_type, vec2 p_pos, vec2 p_size)
    : Rect2(p_pos, p_size), m_type(p_type) {}

EditorSpace::EditorSpace(SplitType p_type, vec2 p_pos, vec2 p_size, double p_proportion)
    : Rect2(p_pos, p_size), m_type(p_type) {
    split(p_proportion);
}

EditorSpace* EditorSpace::get_parent(){
    if(!m_parent){
        return this;
    }
    return m_parent;
}
void EditorSpace::set_parent(EditorSpace* p_space){
    m_parent = p_space;
}

void EditorSpace::unsplit(){
    switch (from) {
        case From::START:{
            m_split = 0.0f;
            break;
        }
        case From::END:{
            m_split = 1.0f;
            break;
        }
    }
    
    refresh_children();
}
double EditorSpace::m_get_total_dist(){
    double ret = (m_type == SplitType::HORIZONTAL) ? get_right_top().x - get_left_top().x : get_left_down().y - get_left_top().y;
    return ret;
}
double EditorSpace::m_proportion_to_value(double p_proportion){
    double total_dist = m_get_total_dist();
    return std::clamp(total_dist * p_proportion, 0.0, total_dist);
}
double EditorSpace::m_value_to_proportion(double p_value){
    return std::clamp(p_value/m_get_total_dist(), 0.0, 1.0);
}
void EditorSpace::m_refresh_split_value_buffer(){
    m_split_value_buffer = m_proportion_to_value(m_split);
}

double EditorSpace::m_get_limited_split(double p_value){
    double f_min;
    double f_max;
    double total_dist = m_get_total_dist();

    /*   ---MIN---   */
    switch (split_limit.min_type) {
        case SplitLimit::Type::PROPORTION:{
            f_min = split_limit.min;
            break;
        }
        case SplitLimit::Type::VALUE:{
            double f_min_prop;
            f_min_prop = m_value_to_proportion(split_limit.min);
            f_min = f_min_prop;
            break;
        }
    }

    /*   ---MAX---   */
    switch (split_limit.max_type) {
        case SplitLimit::Type::PROPORTION:{
            f_max = split_limit.max;
            break;
        }
        case SplitLimit::Type::VALUE:{
            double f_max_prop;
            f_max_prop = m_value_to_proportion(split_limit.max);
            f_max = f_max_prop;
            break;
        }
    }

    if(from == From::END){
        double f_min_buf = f_min;
        double f_max_buf = f_max;

        f_min = std::clamp(1.0 - f_max_buf, 0.0, 1.0);
        f_max = std::clamp(1.0 - f_min_buf, 0.0, 1.0);
    }

    return std::clamp(p_value, f_min, f_max);
}

double EditorSpace::m_get_fixed_split(){
    double ret;

    switch (split_fixed.type) {
        case SplitFixed::Type::PROPORTION:{
            ret = split_fixed.value;
            break;
        }
        case SplitFixed::Type::VALUE:{
            vec2 left_top = this->get_left_top();
            vec2 query_pos = left_top + vec2(split_fixed.value, split_fixed.value);
            ret = get_proportion(query_pos);
            break;
        }
    }

    if(from == From::END){
        ret = 1 - ret;
    }

    return ret;
}

void EditorSpace::m_try_create_children(){
    if(m_spaces.size() == 0){
        m_spaces.push_back(EditorSpace());
        m_spaces.push_back(EditorSpace());

        m_spaces[0].set_parent(this);
        m_spaces[1].set_parent(this);
    }
}

void EditorSpace::split(){
    for(SplitMagnet& m : split_magnets){
        apply_magnet(m,m_split);
    }
    if(split_fixed.is_enabled()){
        m_split = this->m_get_fixed_split();
    }
    if(split_limit.is_enabled()){
        m_split = this->m_get_limited_split(m_split);
    }
    m_try_create_children();
    refresh_children();
    //m_refresh_split_value_buffer();
}

void EditorSpace::split(double m_proportion){
    for(SplitMagnet& m : split_magnets){
        apply_magnet(m,m_proportion);
    }
    if(split_fixed.is_enabled()){
        m_proportion = this->m_get_fixed_split();
    }
    if(split_limit.is_enabled()){
        m_proportion = this->m_get_limited_split(m_proportion);
    }

    m_split = m_proportion;

    m_try_create_children();
    refresh_children();
    m_refresh_split_value_buffer();
}

void EditorSpace::set_type(SplitType p_type){
    m_type = p_type;
    refresh_children();
    m_refresh_split_value_buffer();
}

EditorSpace::Children EditorSpace::get_children(){
    Children ret;

    if(!m_spaces.empty()){
        ret.first = &m_spaces[0];
        ret.second = &m_spaces[1];
    }

    return  ret;
}
bool EditorSpace::has_children(){
    return !m_spaces.empty();
}

std::vector<EditorSpace> EditorSpace::get_spaces() const{
    if(m_spaces.empty()){
        return {*this};
    }

    std::vector<EditorSpace> ret;
    
    ret.push_back(*this);

    for(EditorSpace s : m_spaces){
        auto s_cd = s.get_spaces();
        ret.push_back(s);

        if(s_cd.size() == 1){
            continue;
        }

        for(EditorSpace s2 : s_cd){
            ret.push_back(s2);
        }
    }

    return ret;
}
std::vector<EditorSpace*> EditorSpace::get_spaces_ptr(){
    std::vector<EditorSpace*> ret;

    if(m_spaces.empty()){
        ret.push_back(this);
        return ret;
    }

    ret.push_back(this);
    
    for(EditorSpace& s : m_spaces){
        auto s_cd = s.get_spaces_ptr();
        ret.push_back(&s);

        if(s_cd.size() == 1){
            continue;
        }

        for(EditorSpace* s2 : s_cd){
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

void EditorSpace::restore_buffer_value(){
    split(m_value_to_proportion(m_split_value_buffer));
}