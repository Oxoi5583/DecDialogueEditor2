#include "obj/graph/repeater.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "engine/renderer.h"
#include "obj/abstract/movable.h"
#include "obj/abstract/selectable.h"
#include "obj/graph/base.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include "theme/theme_loader.h"
#include <vector>


Rect2& GraphRepeater::m_init_shape(){
    Rect2 shape = {vec2(0.0f, 0.0f), vec2(10.0f, 10.0f)};
    this->set_shape(shape);
    return this->get_shape<Rect2>();
}

GraphRepeater::GraphRepeater()
: m_rect(m_init_shape()){
    BIND_CLASS(GraphRepeater);
}
GraphRepeater::~GraphRepeater(){}

void GraphRepeater::ready(){}
void GraphRepeater::pre_process(){
    m_refresh_from_and_to();
}
void GraphRepeater::process(){}
void GraphRepeater::post_process(){}
void GraphRepeater::draw(){}

void GraphRepeater::m_refresh_from_and_to(){
    m_from_ids = this->skip_from_repeater();
    m_to_ids = this->skip_to_repeater();
}

GraphManager::NodeType GraphRepeater::get_type(){
    return GraphManager::NodeType::REPEATER;
}

std::vector<OID> GraphRepeater::get_repeater_from(){
    return m_from_ids;
}
std::vector<OID> GraphRepeater::get_repeater_to(){
    return m_to_ids;
}