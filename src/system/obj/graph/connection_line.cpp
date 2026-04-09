#include "system/obj/graph/connection_line.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "engine/renderer.h"
#include "glm/geometric.hpp"
#include "system/graph/camera.h"
#include "system/obj/abstract/movable.h"
#include "system/obj/graph/base.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/line.h"
#include <cmath>
#include <server/mouse_server.h>
#include <server/physics_server.h>
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
    m_remove_if_parent_removed();
    m_update_z_index();
    m_update_pos_buffer();
    m_update_pos();
}
void GraphConnectionLine::draw(){
    vec4 colour = ThemeLoader::Ref()->get_color("AccentColour1");
    double zoom = std::min(GraphCamera::Ref()->get_zoom(), 1.0f);
    
    vec2 dir = glm::normalize(m_to_pos - m_from_pos);
    vec2 rev_dir = glm::normalize(m_from_pos - m_to_pos);

    glm::mat4 left_trans = glm::rotate(glm::mat4(1.0f), 75.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 right_trans = glm::rotate(glm::mat4(1.0f), -75.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    vec2 left_dir = left_trans * vec4(rev_dir, 0.0f, 1.0f);
    vec2 right_dir = right_trans * vec4(rev_dir, 0.0f, 1.0f);

    vec2 size = glm::min(m_arrow_size / vec2(zoom, zoom), m_arrow_size_max);

    float edge = std::sqrt(std::pow((size.x / 2.0f), 2) * std::pow((size.y / 2.0f), 2));
    m_center_pos = m_from_pos + ((m_to_pos - m_from_pos) / 2.0f);
    m_arrow_pos_middle = m_center_pos + (dir * (size / 2.0f));
    m_arrow_pos_left = m_arrow_pos_middle + left_dir * edge;
    m_arrow_pos_right = m_arrow_pos_middle + right_dir * edge;

    EngineRenderer::Ref()->draw_line(
        m_line.get_from()
        , m_line.get_to()
        , colour
        , m_line.get_width() / zoom
    );
    if(!this->is_dragging()){
        EngineRenderer::Ref()->draw_line(
            m_arrow_pos_middle
            , m_arrow_pos_left
            , colour
            , m_line.get_width() * 0.75f / zoom
        );
        EngineRenderer::Ref()->draw_line(
            m_arrow_pos_middle
            , m_arrow_pos_right
            , colour
            , m_line.get_width() * 0.75f / zoom
        );
    }
}

void GraphConnectionLine::m_remove_if_double_clicked(){
    if(this->was_just_double_clicked()){
        this->queue_free();
    }
}
void GraphConnectionLine::m_remove_if_parent_removed(){
    if(!ObjectServer::Ref()->is_id_valid(m_from_id)
        || !ObjectServer::Ref()->is_id_valid(m_to_id)){
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
}
void GraphConnectionLine::m_update_pos(){
    if(!ObjectServer::Ref()->is_id_valid(m_from_id)
        || !ObjectServer::Ref()->is_id_valid(m_to_id)){
        return;
    }

    MovableObject* f = ObjectServer::Ref()->get_instance<MovableObject>(m_from_id);
    MovableObject* t = ObjectServer::Ref()->get_instance<MovableObject>(m_to_id);
    if(is_dragging()){
        f->set_position(m_line.get_from());
        t->set_position(m_line.get_to());
    }else{
        m_line.set_from(f->get_position());
        m_line.set_to(t->get_position());
    }
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
