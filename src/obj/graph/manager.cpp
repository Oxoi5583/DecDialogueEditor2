#include "obj/graph/manager.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/graph/node.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"

GraphManager::GraphManager(){
    BIND_CLASS(GraphManager);
}
GraphManager::~GraphManager(){

}

void GraphManager::m_spawn_entry(vec2 p_pos){
    GraphNode* new_node = ObjectServer::Ref()->queue_create<GraphNode>();
    OID id = new_node->get_id();
    m_all_node_ids.push_back(id);

    new_node->set_position(p_pos);
}
void GraphManager::m_spawn_node(vec2 p_pos){
    GraphNode* new_node = ObjectServer::Ref()->queue_create<GraphNode>();
    OID id = new_node->get_id();
    m_all_node_ids.push_back(id);

    new_node->set_position(p_pos);
}
void GraphManager::m_spawn_option(vec2 p_pos){
    GraphNode* new_node = ObjectServer::Ref()->queue_create<GraphNode>();
    OID id = new_node->get_id();
    m_all_node_ids.push_back(id);

    new_node->set_position(p_pos);
}

void GraphManager::ready(){
}
void GraphManager::pre_process(){
}
void GraphManager::process(){
}
void GraphManager::post_process(){
    auto events = EventServer::Ref()->poll<EventSpawnNode>();
    for(auto event : events){
        vec2 spawn_pos = event.spawn_pos;
        switch (event.type) {
            case ENTRY:{
                m_spawn_entry(spawn_pos);
                break;
            }
            case NODE:{
                m_spawn_node(spawn_pos);
                break;
            }
            case OPTION:{
                m_spawn_option(spawn_pos);
                break;
            }
        }
    }
}
void GraphManager::draw(){
    
}