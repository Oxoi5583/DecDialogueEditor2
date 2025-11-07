#pragma once

#include "DecToolsBox/abstract./singleton.h"

class GraphBackground : public Singleton<GraphBackground>{
private:
    void m_emit_event_if_hit_background();
public:
    void init();
    void update();
};