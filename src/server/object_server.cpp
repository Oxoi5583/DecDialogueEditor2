#include "server/object_server.h"
#include "DecToolsBox/container/ordered_list.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_base.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <set>
#include <vector>


void ObjectServer::ready(){
    for(auto it = m_process_list.begin(); it != m_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (ready) : " << id);
        if(it->is_alive() && !it->is_ready()){
            auto& functions = m_ready_functions[id];
            for(auto function : functions){
                function();
            }

            it->m_is_ready = true;
        }
    }
}
void ObjectServer::pre_process(){
    for(auto it = m_process_list.begin(); it != m_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (pre_process) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_pre_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}
void ObjectServer::process(){
    for(auto it = m_process_list.begin(); it != m_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (process) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}
void ObjectServer::post_process(){
    for(auto it = m_process_list.begin(); it != m_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (post_process) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_post_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}
void ObjectServer::draw(){
    for(auto it = m_process_list.rev_begin(); it != m_process_list.rev_end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (draw) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_draw_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}

void ObjectServer::m_add_buffer_to_process_list(){
    while(!m_obj_buffer.empty()){
        ObjectBase* ptr = m_obj_buffer.front();
        OID new_id = ptr->get_id();
        m_process_list.push_front(ptr);
        m_obj_buffer.pop();
    }
}

void ObjectServer::clear_garbage(){
    m_add_buffer_to_process_list();

    std::unordered_set<OID> garbages;
    for(auto& it : m_instances){
        if(!it->is_alive()){
            OID id = it->get_id();
            ObjectBase* ptr = it.get();
            
            garbages.emplace(id);
            
            m_process_list.erase(ptr);
            m_id_to_instances.erase(id);
            m_ready_functions.erase(id);
            m_pre_process_functions.erase(id);
            m_process_functions.erase(id);
            m_post_process_functions.erase(id);
            m_draw_functions.erase(id);
        }
    }
    m_instances.erase(
        std::remove_if(
                m_instances.begin(),
                m_instances.end(),
                [garbages](std::unique_ptr<ObjectBase>& obj){
                        return garbages.contains(obj->get_id());
                    }
            ),
        m_instances.end()
    );
}

bool ObjectServer::is_id_valid(OID p_id){
    if(!m_id_to_instances.contains(p_id)){
        return false;
    }

    if(!m_id_to_instances[p_id]->is_alive()){
        return false;
    }

    return true;
}
