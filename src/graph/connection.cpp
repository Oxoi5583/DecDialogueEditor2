#include "graph/connection.h"
#include "DecToolsBox/debug/messenger.h"
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
                m_state = State::NUL;
                break;
            }
            break;
        }
    }
}

bool GraphConnection::m_condition_nul_to_start(){
    if(EventServer::Ref()->has<EventStartConnect>()){
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

void GraphConnection::init(){
    
}
void GraphConnection::pre_update(){
    m_update_state();
    m_emit_event();
}
void GraphConnection::post_update(){

}
void GraphConnection::draw(){

}

bool GraphConnection::is_connecting(){
    return m_state != State::NUL;
}