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
    for(auto it = m_ui_process_list.begin(); it != m_ui_process_list.end(); ++it){
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
    for(auto it = m_graph_process_list.begin(); it != m_graph_process_list.end(); ++it){
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
    for(auto it = m_ui_process_list.begin(); it != m_ui_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (pre_process) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_pre_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
    for(auto it = m_graph_process_list.begin(); it != m_graph_process_list.end(); ++it){
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
    for(auto it = m_ui_process_list.begin(); it != m_ui_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (process) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
    for(auto it = m_graph_process_list.begin(); it != m_graph_process_list.end(); ++it){
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
    for(auto it = m_ui_process_list.begin(); it != m_ui_process_list.end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (post_process) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_post_process_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
    for(auto it = m_graph_process_list.begin(); it != m_graph_process_list.end(); ++it){
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
    for(auto it = m_ui_process_list.rev_begin(); it != m_ui_process_list.rev_end(); ++it){
        OID id = it->get_id();
        //DEBUG_MSG("OID (draw) : " << id);
        if(it->is_alive() && it->is_ready()){
            auto& functions = m_draw_functions[id];
            for(auto function : functions){
                function();
            }
        }
    }
    for(auto it = m_graph_process_list.rev_begin(); it != m_graph_process_list.rev_end(); ++it){
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
        std::pair<ObjectBase*, Layer> it = m_obj_buffer.front();
        ObjectBase* ptr = it.first;
        Layer layer = it.second;
        OID new_id = ptr->get_id();
        switch (layer) {
            case Layer::UI_LAYER:{
                m_ui_process_list.push_front(ptr);
                break;
            }
            case Layer::GRAPH_LAYER:{
                m_graph_process_list.push_front(ptr);
                break;
            }
        }
        
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
            
            m_graph_process_list.erase(ptr);
            m_id_to_instances.erase(id);
            m_ready_functions.erase(id);
            m_pre_process_functions.erase(id);
            m_process_functions.erase(id);
            m_post_process_functions.erase(id);
            m_draw_functions.erase(id);
            m_layers.erase(id);
        }
    }
    m_instances.erase(
        std::remove_if(
                m_instances.begin(),
                m_instances.end(),
                [garbages](std::unique_ptr<ObjectBase>& obj){
                        OID id = obj->get_id();
                        bool is_garbage = garbages.contains(id);
                        return is_garbage;
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


void ObjectServer::move_to_front(OID p_id){
    ReorderCommand c = {Direction::FRONT, p_id, 0};
    m_commands.emplace(c);
}
void ObjectServer::move_to_back(OID p_id){
    ReorderCommand c = {Direction::BACK, p_id, 0};
    m_commands.emplace(c);
}
void ObjectServer::move_to_specific_front(OID p_id, OID p_target_id){
    ReorderCommand c = {Direction::FRONT, p_id, p_target_id};
    m_commands.emplace(c);
}
void ObjectServer::move_to_specific_back(OID p_id, OID p_target_id){
    ReorderCommand c = {Direction::BACK, p_id, p_target_id};
    m_commands.emplace(c);
}

ordered_list<ObjectBase>& ObjectServer::m_layer_to_list(Layer p_layer){
    switch (p_layer) {
        case Layer::UI_LAYER:
            return m_ui_process_list;
            break;
        case Layer::GRAPH_LAYER:
            return m_graph_process_list;
            break;
    }
}
void ObjectServer::reorder(){
    while (!m_commands.empty()) {
        auto& c = m_commands.front();

        if(!is_id_valid(c.id)){
            m_commands.pop();
            continue;
        }

        Layer layer = m_layers[c.id];
        ObjectBase* ptr = this->get_instance<ObjectBase>(c.id);
        auto& list = m_layer_to_list(layer);

        switch (c.dir) {
                case FRONT:{
                    list.move_to_front(ptr);
                    break;
                }
                case BACK:{
                    list.move_to_back(ptr);
                    break;
                }
                case FRONT_SPEC:{
                    if(!is_id_valid(c.target_id)){
                        m_commands.pop();
                        continue;
                    }
                    ObjectBase* target_ptr = this->get_instance<ObjectBase>(c.target_id);
                    list.move_to_specific_front(ptr, target_ptr);
                    break;
                }
                case BACK_SPEC:{
                    if(!is_id_valid(c.target_id)){
                        m_commands.pop();
                        continue;
                    }
                    ObjectBase* target_ptr = this->get_instance<ObjectBase>(c.target_id);
                    list.move_to_specific_back(ptr, target_ptr);
                    break;
                }
            }
        m_commands.pop();
    }
}

ObjectServer::Layer ObjectServer::get_layer(OID p_id){
    if(!is_id_valid(p_id)){
        return ObjectServer::Layer::GRAPH_LAYER;
    }

    return m_layers[p_id];
}