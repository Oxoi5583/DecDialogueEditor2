#include "graph/connection.h"
#include "DecToolsBox/debug/messenger.h"
#include "obj/graph/connection_line.h"
#include "obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"

void GraphConnection::m_update_state(){
    switch (m_state) {
        case NUL:{
            if(m_condition_nul_to_start()){
                m_state = State::START_CONNECT;
                break;
            }
            break;
        }
        case START_CONNECT:{
            if(m_condition_start_to_search()){
                m_state = State::SEARCH_CONNECT;
                break;
            }
            break;
        }
        case SEARCH_CONNECT:{
            if(m_condition_search_to_placed()){
                m_state = State::PLACED_CONNECT;
                break;
            }
            if(m_condition_search_to_nul()){
                m_state = State::NUL;
                break;
            }
            break;
        }
        case PLACED_CONNECT:{
            if(m_condition_placed_to_nul()){
                m_create_connection_obj();
                m_state = State::NUL;
                break;
            }
            break;
        }
    }
}

bool GraphConnection::m_condition_nul_to_start(){
    if(EventServer::Ref()->has<EventStartConnect>()){
        m_start_id = EventServer::Ref()->poll_first<EventStartConnect>().id;
        return true;
    }
    return false;
}
bool GraphConnection::m_condition_start_to_search(){
    return true;
}
bool GraphConnection::m_condition_search_to_placed(){
    if(!MouseServer::Ref()->is_mouse_in_viewport()){
        return false;
    }

    OID hovered_id = EventServer::Ref()->poll_first<EventMouseHoverObjLastFrame>().obj_id;
    if(!ObjectServer::Ref()->is_id_valid(hovered_id)){
        return false;
    }

    ObjectBase* hovered_obj = ObjectServer::Ref()->get_instance<ObjectBase>(hovered_id);
    if(hovered_obj->get_layer() != (int)ObjectServer::Layer::GRAPH_LAYER){
        return false;
    }

    if(!MouseServer::Ref()->is_just_clicked()){
        return false;
    }

    m_end_id = hovered_id;
    return true;
}
bool GraphConnection::m_condition_search_to_nul(){
    if(!MouseServer::Ref()->is_mouse_in_viewport()){
        return false;
    }
    if(!EventServer::Ref()->has<EventMouseHoverOnWorld>()){
        return false;
    }
    if(!MouseServer::Ref()->is_just_clicked()){
        return false;
    }

    return true;
}
bool GraphConnection::m_condition_placed_to_nul(){
    return true;
}

void GraphConnection::m_emit_event(){
    switch (m_state) {
        case NUL:{
            break;
        }
        case START_CONNECT:{
            EventServer::Ref()->emit(EventLockedAll());
            break;
        }
        case SEARCH_CONNECT:{
            EventServer::Ref()->emit(EventLockedAll());
            break;
        }
        case PLACED_CONNECT:{
            EventServer::Ref()->emit(EventLockedAll());
            break;
        }
    }
}

void GraphConnection::m_handle_event(){
    if(EventServer::Ref()->has<EventCreateConnection>()){
        auto events = EventServer::Ref()->poll<EventCreateConnection>();
        for(auto event : events){
            if(!m_connections.contains(event.fm_id)){
                m_connections.emplace(event.fm_id, std::set<OID>());
            }

            m_connections[event.fm_id].emplace(event.to_id);
        }
    }

    if(EventServer::Ref()->has<EventRemoveConnection>()){
        auto events = EventServer::Ref()->poll<EventRemoveConnection>();
        for(auto event : events){
            DEBUG_MSG("EventRemoveConnection : " << event.fm_id);
            if(!m_connections.contains(event.fm_id)){
                continue;
            }
            
            m_connections[event.fm_id].erase(event.to_id);

            if(m_connections[event.fm_id].size() == 0){
                m_connections.erase(event.fm_id);
            }
        }
    }
}

void GraphConnection::init(){
    
}
void GraphConnection::pre_update(){
    m_update_state();
    m_emit_event();
    m_handle_event();
}
void GraphConnection::post_update(){

}
void GraphConnection::draw(){

}

bool GraphConnection::is_connecting(){
    return m_state != State::NUL;
}

void GraphConnection::m_create_connection_obj(){
    GraphConnectionLine* line_obj = ObjectServer::Ref()->queue_create<GraphConnectionLine>();
    line_obj->set_from_id(m_start_id);
    line_obj->set_to_id(m_end_id);
}

std::set<OID> GraphConnection::get_connection(OID p_id){
    if(!m_connections.contains(p_id)){
        return std::set<OID>();
    }
    return m_connections[p_id];
}