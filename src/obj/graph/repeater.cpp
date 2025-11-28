#include "obj/graph/repeater.h"
#include "DecToolsBox/debug/messenger.h"
#include "core/timer_server.h"
#include "engine/renderer.h"
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
void GraphRepeater::pre_process(){}
void GraphRepeater::process(){
    /*
    **** Should have better pass repeater api!!
    
    */
    if(is_selected()){
        OID fm = this->skip_from_repeater();
        std::vector<OID> to = ObjectServer::Ref()->get_instance<GraphBase>(fm)->skip_to_repeater();
        to.push_back(fm);

        for(OID id : to){
            vec2 pos = ObjectServer::Ref()->get_instance<SelectableObject>(id)->get_position();
            EngineRenderer::Ref()->draw_circle(pos, 25, vec4(1.0f,1.0f,0.0f,1.0f), -1);
        }
    }
}
void GraphRepeater::post_process(){}
void GraphRepeater::draw(){}

GraphManager::NodeType GraphRepeater::get_type(){
    return GraphManager::NodeType::REPEATER;
}
