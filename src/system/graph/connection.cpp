#include "system/graph/connection.h"
#include "DecToolsBox/core/condition.h"
#include "DecToolsBox/debug/messenger.h"
#include "system/obj/abstract/movable.h"
#include "system/obj/graph/connection_line.h"
#include "system/obj/graph/manager.h"
#include "system/obj/graph/node.h"
#include "system/obj/graph/repeater.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <algorithm>
#include <vector>

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
                create_connection_obj(m_start_id, m_end_id);
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

    if(!m_connectables.contains(hovered_id)){
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
    if(m_connectables.empty()){
        return true;
    }
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
            m_refresh_screen_ids();
            m_refresh_connectables();
            EventServer::Ref()->emit(EventLockedAll());
            break;
        }
        case SEARCH_CONNECT:{
            EventTryConnectTo event;
            event.conntectables = m_connectables;
            EventServer::Ref()->emit(event);
            EventServer::Ref()->emit(EventLockedAll());
            break;
        }
        case PLACED_CONNECT:{
            std::vector<OID>().swap(m_on_screen_ids);
            std::unordered_set<OID>().swap(m_connectables);
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

void GraphConnection::create_connection_obj(OID p_fm_id, OID p_to_id){
    GraphConnectionLine* line_obj = ObjectServer::Ref()->queue_create<GraphConnectionLine>();
    line_obj->set_from_id(p_fm_id);
    line_obj->set_to_id(p_to_id);
}

std::set<OID> GraphConnection::get_connection(OID p_id){
    if(!m_connections.contains(p_id)){
        return std::set<OID>();
    }
    return m_connections[p_id];
}

void GraphConnection::m_refresh_screen_ids(){
    std::vector<OID>().swap(m_on_screen_ids);
    std::vector<OID> ids = ObjectServer::Ref()->get_all_ids();
    for(OID& id : ids){
        MovableObject* obj = ObjectServer::Ref()->get_instance<MovableObject>(id);
        if(obj){
            if(obj->is_on_camera()){
                m_on_screen_ids.push_back(id);
            }
        }
    }
}
void GraphConnection::m_refresh_connectables(){
    std::unordered_set<OID>().swap(m_connectables);
    for(OID& id : m_on_screen_ids){
        if(test_connection(m_start_id, id)){
            m_connectables.emplace(id);
        }
    }
}

bool GraphConnection::m_test_connection__target_not_self(GraphBase* p_fm, GraphBase* p_to){
    return p_fm->get_id() != p_to->get_id();
}
bool GraphConnection::m_test_connection__target_not_entry(GraphBase* p_fm, GraphBase* p_to){
    return p_to->get_type() != GraphManager::ENTRY;
}
bool GraphConnection::m_test_connection__target_not_connected(GraphBase* p_fm, GraphBase* p_to){
    std::vector<OID> ancestor_ids = p_fm->skip_from_repeater();
    for(OID& ancestor_id : ancestor_ids){
        GraphBase* ancestor = ObjectServer::Ref()->get_instance<GraphBase>(ancestor_id);
        std::set<OID> children = ancestor->get_children_set(true);
        if(children.contains(p_to->get_id())){
            return false;
        }
    }
    return true;
}
bool GraphConnection::m_test_connection__target_not_parent_directly(GraphBase* p_fm, GraphBase* p_to){
    std::set<OID> ancestor_ids = p_fm->get_parent_set(false);
    if(ancestor_ids.contains(p_to->get_id())){
        return false;
    }else{
        return true;
    }
}
bool GraphConnection::m_test_connection__target_not_parent_proxy(GraphBase* p_fm, GraphBase* p_to){
    std::set<OID> ancestor_ids = p_fm->get_parent_set(true, false);
    if(ancestor_ids.contains(p_to->get_id()) && p_fm->get_type() == GraphManager::NODE){
        return false;
    }else{
        return true;
    }
}
bool GraphConnection::m_test_connection__to_repeater_not_have_parent(GraphBase* p_fm, GraphBase* p_to){
    if(p_to->get_type() == GraphManager::REPEATER){
        std::set<OID> parent = p_to->get_parent_set();
        return parent.empty();
    }
    return true;
}
bool GraphConnection::m_test_connection__fm_repeater_not_have_children(GraphBase* p_fm, GraphBase* p_to){
    if(p_fm->get_type() == GraphManager::REPEATER){
        std::set<OID> children = p_fm->get_children_set();
        return children.empty();
    }
    return true;
}
bool GraphConnection::m_test_connection__not_connect_to_normal_node_yet(GraphBase* p_fm, GraphBase* p_to){
    std::vector<OID> check_list;

    auto set = p_fm->get_children(true);
    for(OID id : set){
        GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(obj->get_type() == GraphManager::NODE){
            return false;
        }
    }

    return true;
}
bool GraphConnection::m_test_connection__all_connect_type_is_option(GraphBase* p_fm, GraphBase* p_to){
    bool is_connected_to_option = false;

    auto set = p_fm->get_children(true);
    for(OID id : set){
        GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(obj->get_type() == GraphManager::OPTION){
            is_connected_to_option = true;
        }
    }

    if(is_connected_to_option){
        GraphManager::NodeTypeId to_type = p_to->get_type();
        if(to_type != GraphManager::OPTION && to_type != GraphManager::REPEATER){
            return false;
        }
    }

    return true;
}
bool GraphConnection::m_test_connection__target_not_module_entry(GraphBase* p_fm, GraphBase* p_to){
    return p_to->get_type() != GraphManager::MODULE_ENTRY;
}
bool GraphConnection::m_test_connection__target_not_parent_proxy_module_node(GraphBase* p_fm, GraphBase* p_to){
    std::set<OID> ancestor_ids = p_fm->get_parent_set(true, false);
    if(ancestor_ids.contains(p_to->get_id()) && p_fm->get_type() == GraphManager::MODULE_NODE){
        return false;
    }else{
        return true;
    }
}
bool GraphConnection::m_test_connection__not_connect_to_module_normal_node_yet(GraphBase* p_fm, GraphBase* p_to){
    std::vector<OID> check_list;

    auto set = p_fm->get_children(true);
    for(OID id : set){
        GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(obj->get_type() == GraphManager::MODULE_NODE){
            return false;
        }
    }

    return true;
}


bool GraphConnection::test_connection(OID p_fm_id, OID p_to_id){
    GraphBase* fm_node = ObjectServer::Ref()->get_instance<GraphBase>(p_fm_id);
    GraphBase* to_node = ObjectServer::Ref()->get_instance<GraphBase>(p_to_id);
    if(to_node == nullptr || fm_node == nullptr){
        return false;
    }

    Condition c_1 = Condition()
            .add_required([this, fm_node, to_node](){ return this->m_test_connection__target_not_self(fm_node, to_node); })
            .add_required([this, fm_node, to_node](){ return this->m_test_connection__target_not_entry(fm_node, to_node); })
            .add_required([this, fm_node, to_node](){ return this->m_test_connection__target_not_module_entry(fm_node, to_node); })
            .add_required([this, fm_node, to_node](){ return this->m_test_connection__target_not_connected(fm_node, to_node); })
            .add_required([this, fm_node, to_node](){ return this->m_test_connection__target_not_parent_directly(fm_node, to_node); });

    if(!c_1){
        return false;
    }


    std::vector<GraphBase*> fm_nodes;
    if(fm_node->get_type() == GraphManager::REPEATER){
        std::vector<OID> ids_skip_repeater = fm_node->skip_from_repeater();
        for(OID& id : ids_skip_repeater){
            GraphBase* obj_skip_repeater = ObjectServer::Ref()->get_instance<GraphBase>(id);
            if(obj_skip_repeater){
                fm_nodes.push_back(obj_skip_repeater);
            }
        }
    }else{
        fm_nodes.push_back(fm_node);
    }

    for(GraphBase* fm_sub_node : fm_nodes){
        std::vector<GraphBase*> to_nodes;
        if(to_node->get_type() == GraphManager::REPEATER){
            std::vector<OID> ids_skip_repeater = to_node->skip_to_repeater();
            for(OID& id : ids_skip_repeater){
                GraphBase* obj_skip_repeater = ObjectServer::Ref()->get_instance<GraphBase>(id);
                if(obj_skip_repeater){
                    to_nodes.push_back(obj_skip_repeater);
                }
            }
        }else{
            to_nodes.push_back(to_node);
        }

        for(GraphBase* to_node_2 : to_nodes){
            Condition c_2 = Condition()
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__target_not_self(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__target_not_entry(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__target_not_connected(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__target_not_parent_proxy(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__not_connect_to_normal_node_yet(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__all_connect_type_is_option(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__not_connect_to_module_normal_node_yet(fm_sub_node, to_node_2); })
                .add_required([this, fm_sub_node, to_node_2](){ return this->m_test_connection__target_not_parent_proxy_module_node(fm_sub_node, to_node_2); })
                .add_alternative(1, [this, fm_sub_node, to_node_2](){ return this->m_test_connection__fm_repeater_not_have_children(fm_sub_node, to_node_2); });

            bool c_2_ret = c_2.result();

            if(!c_2_ret){
                return false;
            }

        }
    }
    return true;
}