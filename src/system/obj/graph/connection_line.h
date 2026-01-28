#pragma once

#include "server/timer_server.h"
#include "engine/renderer.h"
#include "glm/ext/vector_float2.hpp"
#include "system/obj/abstract/clickable.h"
#include "system/obj/abstract/dragable.h"
#include "system/obj/abstract/hoverable.h"
#include "system/obj/abstract/selectable.h"
#include "system/obj/graph/manager.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "struct/shape/line.h"
#include "struct/shape/rect2.h"
#include <vector>

using namespace glm;

class GraphConnectionLine : public DragableObject{
private:
    Line& m_init_shape();
    Line& m_line;

    OID m_from_id;
    OID m_to_id;

    vec2 m_center_pos;
    vec2 m_from_pos;
    vec2 m_to_pos;
    
    const vec2 m_arrow_size = {5.0f, 5.0f};
    const vec2 m_arrow_size_max = {10.0f, 10.0f};
    
    vec2 m_arrow_pos_middle;
    vec2 m_arrow_pos_left;
    vec2 m_arrow_pos_right;

    void m_update_pos_buffer();
    void m_update_pos();
    void m_remove_if_double_clicked();
    void m_remove_if_parent_removed();
    void m_update_z_index();

public:
    GraphConnectionLine();
    ~GraphConnectionLine();

    OID get_from_id() const;
    OID get_to_id() const;
    void set_from_id(OID p_id);
    void set_to_id(OID p_id);

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};
