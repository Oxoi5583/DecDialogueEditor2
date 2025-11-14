#pragma once

#include "core/timer_server.h"
#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/abstract/clickable.h"
#include "obj/abstract/dragable.h"
#include "obj/abstract/hoverable.h"
#include "obj/abstract/selectable.h"
#include "obj/graph/manager.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"
#include <vector>

using namespace glm;

class GraphBase : public SelectableObject{
private:
    std::string m_name;
    std::string m_content;
    std::vector<std::string> m_signals;
    std::set<OID> m_children;

    Rect2& m_rect;
    Rect2& m_init_shape();
public:
    GraphBase();
    ~GraphBase();

    vec2 get_position() const;
    vec2 get_size() const;
    void set_position(vec2& p_position);
    void set_size(vec2& p_size);

    bool is_point_intersect(vec2& p_point); 

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    virtual GraphManager::NodeType get_type();

    std::string get_name();
    std::string get_content();
    std::vector<std::string> get_signals();
    std::vector<OID> get_children();

    void set_name(std::string p_name);
    void set_content(std::string p_content);
    void add_signals();
    void remove_signals(int p_index);
    void set_signal(int p_index, std::string p_signal);
    void add_children(OID p_id);
    void remove_children(OID p_id);
};
