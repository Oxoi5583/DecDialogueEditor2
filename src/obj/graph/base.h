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
#include "server/object_server.h"
#include "struct/shape/rect2.h"
#include <map>
#include <string>
#include <vector>

using namespace glm;

class GraphBase : public SelectableObject{
public:
    struct Property{
        std::string value;
        uint max_size;
    };

private:
    std::vector<std::string> m_signals;
    std::set<OID> m_children;
    std::set<OID> m_parent;

    std::map<std::string,Property> m_properties = {
        {"Name",{"", 50}},
        {"Content",{ "", 1000}},
    };

    Rect2& m_init_shape();

    void m_handle_event_connect();
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

    void set_name(std::string p_name);
    void set_content(std::string p_content);
    void add_signals();
    void remove_signals(int p_index);
    void set_signal(int p_index, std::string p_signal);

    void add_children(OID p_id);
    void remove_children(OID p_id);

    void add_parent(OID p_id);
    void remove_parent(OID p_id);

    OID skip_from_repeater();
    std::vector<OID> skip_to_repeater();

    std::vector<OID> get_children(bool is_pass_repeater = false);
    std::vector<OID> get_parent(bool is_pass_repeater = false);
    std::set<OID> get_children_set(bool is_pass_repeater = false);
    std::set<OID> get_parent_set(bool is_pass_repeater = false);

    std::map<std::string,Property>& get_properties();
};
