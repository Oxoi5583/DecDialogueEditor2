#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <vector>

using namespace glm;

class GraphManager : public ObjectBase {
private:
    std::vector<OID> m_all_node_ids;

    void m_spawn_entry(vec2 p_pos);
    void m_spawn_node(vec2 p_pos);
    void m_spawn_option(vec2 p_pos);
public:
    GraphManager();
    ~GraphManager();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};