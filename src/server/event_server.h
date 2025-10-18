#pragma once

#include "DecToolsBox/abstract/singleton.h"

#include <any>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventServer : public Singleton<EventServer> {
public:
    template<typename EventType>
    std::vector<EventType> poll(){
        if(!m_events_buffer.contains(typeid(EventType))){
            return std::vector<EventType>();
        }
        std::vector<EventType> ret;
        for(const std::any& a : m_events_buffer[typeid(EventType)]){
            ret.push_back(std::any_cast<EventType>(a));
        }
        return ret;
    }
    template<typename EventType>
    bool has(){
        if(!m_events_buffer.contains(typeid(EventType))){
            return false;
        }
        return true;
    }
    template<typename EventType>
    void emit(EventType p_event){
        if(!m_events.contains(typeid(EventType))){
            m_events.emplace(typeid(EventType),std::vector<std::any>());
        }
        m_events[typeid(EventType)].push_back(p_event);
    }
    void flush();
private:
    std::unordered_map<std::type_index, std::vector<std::any>> m_events;
    std::unordered_map<std::type_index, std::vector<std::any>> m_events_buffer;
};