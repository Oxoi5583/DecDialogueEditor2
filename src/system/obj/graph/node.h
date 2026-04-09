#pragma once

#include "server/timer_server.h"
#include "engine/renderer.h"
#include "system/obj/graph/base.h"
#include "system/obj/graph/manager.h"
#include "server/object_base.h"
#include "struct/shape/rect2.h"

using namespace glm;

class GraphNode : public GraphBase{
private:
public:
    GraphNode();
    ~GraphNode();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    GraphManager::NodeTypeId get_type() override;
};
