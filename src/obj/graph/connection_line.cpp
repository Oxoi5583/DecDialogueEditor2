#include "obj/graph/connection_line.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "engine/renderer.h"
#include "glm/geometric.hpp"
#include "obj/abstract/movable.h"
#include "obj/graph/base.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/line.h"
#include <cmath>
#include "theme/theme_loader.h"

GraphConnectionLine::GraphConnectionLine()
: m_line(m_init_shape()){
    BIND_CLASS(GraphConnectionLine);
}
GraphConnectionLine::~GraphConnectionLine(){
    EventRemoveConnection event;
    event.fm_id = this->get_from_id();
    event.to_id = this->get_to_id();
    EventServer::Ref()->emit(event);
}

Line& GraphConnectionLine::m_init_shape(){
    Line line;
    line.set_width(3.0f);
    this->set_shape(line); 
    return this->get_shape<Line>();
}

void GraphConnectionLine::ready(){
    EventCreateConnection event;
    event.fm_id = this->get_from_id();
    event.to_id = this->get_to_id();
    EventServer::Ref()->emit(event);
}
void GraphConnectionLine::pre_process(){
}
void GraphConnectionLine::process(){
}
void GraphConnectionLine::post_process(){
    m_remove_if_double_clicked();
    m_update_z_index();
    m_update_pos_buffer();
    m_update_line_pos();
}
void GraphConnectionLine::draw(){
    vec4 colour = ThemeLoader::Ref()->get_color("AccentColour1");
    EngineRenderer::Ref()->draw_line(
        m_line.get_from()
        , m_line.get_to()
        , colour
        , m_line.get_width()
    );
    EngineRenderer::Ref()->draw_line(
        m_arrow_pos_middle
        , m_arrow_pos_left
        , colour
        , m_line.get_width() * 0.75f
    );
    EngineRenderer::Ref()->draw_line(
        m_arrow_pos_middle
        , m_arrow_pos_right
        , colour
        , m_line.get_width() * 0.75f
    );
}

void GraphConnectionLine::m_remove_if_double_clicked(){
    if(this->was_just_double_clicked()){
        this->queue_free();
    }
}
void GraphConnectionLine::m_update_z_index(){
    if(!ObjectServer::Ref()->is_id_valid(m_from_id)){
        return;
    }

    ObjectServer::Ref()->move_to_specific_back(get_id(), get_from_id());
}
void GraphConnectionLine::m_update_pos_buffer(){
    if(!ObjectServer::Ref()->is_id_valid(m_from_id)
        || !ObjectServer::Ref()->is_id_valid(m_to_id)){
        return;
    }

    MovableObject* f = ObjectServer::Ref()->get_instance<MovableObject>(m_from_id);
    MovableObject* t = ObjectServer::Ref()->get_instance<MovableObject>(m_to_id);

    m_from_pos = f->get_position();
    m_to_pos = t->get_position();

    m_center_pos = m_from_pos + ((m_to_pos - m_from_pos) / 2.0f);

    vec2 dir = glm::normalize(m_to_pos - m_from_pos);
    vec2 rev_dir = glm::normalize(m_from_pos - m_to_pos);

    m_arrow_pos_middle = m_center_pos + (dir * (m_arrow_size / 2.0f));

    glm::mat4 left_trans = glm::rotate(glm::mat4(1.0f), 75.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 right_trans = glm::rotate(glm::mat4(1.0f), -75.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    vec2 left_dir = left_trans * vec4(rev_dir, 0.0f, 1.0f);
    vec2 right_dir = right_trans * vec4(rev_dir, 0.0f, 1.0f);

    float edge = std::sqrt(std::pow((m_arrow_size.x / 2.0f), 2) * std::pow((m_arrow_size.y / 2.0f), 2));

    m_arrow_pos_left = m_arrow_pos_middle + left_dir * edge;
    m_arrow_pos_right = m_arrow_pos_middle + right_dir * edge;
}
void GraphConnectionLine::m_update_line_pos(){
    m_line.set_from(m_from_pos);
    m_line.set_to(m_to_pos);
}

OID GraphConnectionLine::get_from_id() const{
    return m_from_id;
}
OID GraphConnectionLine::get_to_id() const{
    return m_to_id;
}
void GraphConnectionLine::set_from_id(OID p_id){
    m_from_id = p_id;
}
void GraphConnectionLine::set_to_id(OID p_id){
    m_to_id = p_id;
}
