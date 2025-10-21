#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "DecToolsBox/container/ordered_list.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/object_base.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ObjectServer : public Singleton<ObjectServer> {
private:

    ordered_list<ObjectBase> m_process_list;

    std::vector<std::unique_ptr<ObjectBase>> m_instances;

    std::unordered_map<OID, ObjectBase*> m_id_to_instances;
    std::unordered_map<OID, std::vector<std::function<void()>>> m_ready_functions;
    std::unordered_map<OID, std::vector<std::function<void()>>> m_pre_process_functions;
    std::unordered_map<OID, std::vector<std::function<void()>>> m_process_functions;
    std::unordered_map<OID, std::vector<std::function<void()>>> m_post_process_functions;
    std::unordered_map<OID, std::vector<std::function<void()>>> m_draw_functions;

    std::queue<ObjectBase*> m_obj_buffer;
    void m_add_buffer_to_process_list();
public:

    bool is_id_valid(OID p_id);

    enum class FunctionType{
        FUNC_READY,
        FUNC_PRE_PROCESS,
        FUNC_PROCESS,
        FUNC_POST_PROCESS,
        FUNC_DRAW,
    };

    template<typename T>
    void register_function(FunctionType p_type, std::function<void()> p_func, T* p_ptr){
        static_assert( std::is_base_of<ObjectBase, T>::value, "Only object derived can register function." );

        OID id = p_ptr->get_id();


        switch (p_type) {
            case FunctionType::FUNC_READY:{
                DEBUG_MSG("Register ID (FUNC_READY) : " << id);
                m_ready_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_PRE_PROCESS:{
                DEBUG_MSG("Register ID (FUNC_PRE_PROCESS) : " << id);
                m_pre_process_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_PROCESS:{
                DEBUG_MSG("Register ID (FUNC_PROCESS) : " << id);
                m_process_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_POST_PROCESS:{
                DEBUG_MSG("Register ID (FUNC_POST_PROCESS) : " << id);
                m_post_process_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_DRAW:{
                DEBUG_MSG("Register ID (FUNC_DRAW) : " << id);
                m_draw_functions[id].push_back(p_func);
                break;
            }
        }
    }

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();

    void clear_garbage();

    template<typename T>
    T* queue_create(){
        static_assert( std::is_base_of<ObjectBase, T>::value, "Only object derived can be created." );

        m_instances.push_back(std::make_unique<T>());
        T* ptr = static_cast<T*>(m_instances.back().get());
        m_obj_buffer.push(ptr);
        OID new_id = ptr->get_id();

        m_id_to_instances.emplace(new_id, ptr);
        m_ready_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_pre_process_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_process_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_post_process_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_draw_functions.emplace(new_id, std::vector<std::function<void()>>());

        return ptr;
    }
};


#define BIND_CLASS(CLASS_NAME) ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_READY, std::bind(&CLASS_NAME::ready, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_PRE_PROCESS, std::bind(&CLASS_NAME::pre_process, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_PROCESS, std::bind(&CLASS_NAME::process, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_POST_PROCESS, std::bind(&CLASS_NAME::post_process, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_DRAW, std::bind(&CLASS_NAME::draw, this), this); \
                        