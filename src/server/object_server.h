#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "DecToolsBox/container/ordered_list.h"
#include "server/object_base.h"
#include <cstdint>
#include <functional>
#include <memory>
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

public:

    bool is_id_valid(OID p_id);

    enum class FunctionType{
        FUNC_READY,
        FUNC_PRE_PROCESS,
        FUNC_PROCESS,
        FUNC_POST_PROCESS,
    };

    template<typename T>
    void register_function(FunctionType p_type, std::function<void()> p_func, T* p_ptr){
        static_assert( std::is_base_of<ObjectBase, T>::value, "Only object derived can register function." );

        OID id = p_ptr->get_id();

        switch (p_type) {
            case FunctionType::FUNC_READY:{
                m_ready_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_PRE_PROCESS:{
                m_pre_process_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_PROCESS:{
                m_process_functions[id].push_back(p_func);
                break;
            }
            case FunctionType::FUNC_POST_PROCESS:{
                m_post_process_functions[id].push_back(p_func);
                break;
            }
        }
    }

    void ready();
    void pre_process();
    void process();
    void post_process();

    void clear_garbage();

    template<typename T>
    T* create_object(){
        static_assert( std::is_base_of<ObjectBase, T>::value, "Only object derived can be created." );

        m_instances.push_back(std::make_unique<T>());
        T* ptr = static_cast<T*>(m_instances.back().get());
        OID new_id = ptr->get_id();

        m_process_list.push_back(ptr);
        m_id_to_instances.emplace(new_id, ptr);
        m_ready_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_pre_process_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_process_functions.emplace(new_id, std::vector<std::function<void()>>());
        m_post_process_functions.emplace(new_id, std::vector<std::function<void()>>());

        return ptr;
    }
};


#define BIND_CLASS(CLASS_NAME) ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_READY, std::bind(&CLASS_NAME::ready, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_PRE_PROCESS, std::bind(&CLASS_NAME::pre_process, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_PROCESS, std::bind(&CLASS_NAME::process, this), this); \
                         ObjectServer::Ref()->register_function(ObjectServer::FunctionType::FUNC_POST_PROCESS, std::bind(&CLASS_NAME::post_process, this), this); \
                        