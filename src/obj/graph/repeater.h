#pragma once

#include "core/timer_server.h"
#include "engine/renderer.h"
#include "obj/graph/base.h"
#include "obj/graph/manager.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"

using namespace glm;

class GraphRepeater : public GraphBase{
private:
    Rect2& m_rect;
    Rect2& m_init_shape();

    OID m_from_id;
    std::vector<OID> m_to_ids;
    void m_refresh_from_and_to();
public:
    GraphRepeater();
    ~GraphRepeater();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    GraphManager::NodeType get_type() override;

    OID get_repeater_from();
    std::vector<OID> get_repeater_to();
};
