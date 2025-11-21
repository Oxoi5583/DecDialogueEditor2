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
#include "struct/shape/line.h"
#include "struct/shape/rect2.h"
#include <vector>

using namespace glm;

class GraphConnectionLine : public ClickableObject{
private:
    Line m_line;
    OID m_from_id;
    OID m_to_id;
    
    void m_update_line_pos();
    void m_handle_event_connect();
public:
    GraphConnectionLine();
    ~GraphConnectionLine();


    OID get_from_id() const;
    OID get_to_id() const;
    void set_from_id(OID p_id);
    void set_to_id(OID p_id);


    bool is_point_intersect(vec2& p_point); 

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

};
