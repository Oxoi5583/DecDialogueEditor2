#include "server/object_server.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_base.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <set>
#include <vector>


void ObjectServer::ready(){
    for(auto it : m_process_list){
        OID id = it->get_id();
        if(it->is_alive() && !it->is_ready()){
            auto& functions = m_ready_functions[id];
            for(auto function : functions){
                function();
            }

            it->m_is_ready = false;
        }
    }
}
void ObjectServer::pre_process(){
    //DEBUG_MSG("m_process_list size : " << m_process_list.size());
    for(auto it : m_process_list){
        OID id = it->get_id();
        if(it->is_alive()){
            auto& functions = m_pre_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}
void ObjectServer::process(){
    for(auto it : m_process_list){
        OID id = it->get_id();
        if(it->is_alive()){
            auto& functions = m_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}
void ObjectServer::post_process(){
    for(auto it : m_process_list){
        OID id = it->get_id();
        if(it->is_alive()){
            auto& functions = m_post_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
}


void ObjectServer::clear_garbage(){
    std::unordered_set<OID> garbages;
    for(auto& it : m_instances){
        if(!it->is_alive()){
            OID id = it->get_id();
            ObjectBase* ptr = it.get();

            DEBUG_MSG("Clear Id : " << id);
            
            garbages.emplace(id);
            
            DEBUG_MSG("Process List Size (Before) : " << m_process_list.size());
            m_process_list.erase(ptr);
            DEBUG_MSG("Process List Size (After) : " << m_process_list.size());
            m_id_to_instances.erase(id);
            m_ready_functions.erase(id);
            m_pre_process_functions.erase(id);
            m_process_functions.erase(id);
            m_post_process_functions.erase(id);
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
