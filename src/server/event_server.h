#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "DecToolsBox/debug/messenger.h"

#include <any>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventServer : public Singleton<EventServer> {
    typedef std::unordered_map<std::type_index, std::vector<std::any>> DataContainer;

public:
    template<typename EventType>
    std::vector<EventType> poll(){
        DataContainer& data = (EventType::is_event_unique) ? m_events : m_events_buffer;

        if(!data.contains(typeid(EventType))){
            return std::vector<EventType>();
        }
        std::vector<EventType> ret;
        for(const std::any& a : data[typeid(EventType)]){
            ret.push_back(std::any_cast<EventType>(a));
        }
        return ret;
    }
    template<typename EventType>
    EventType poll_first(){
        DataContainer& data = (EventType::is_event_unique) ? m_events : m_events_buffer;

        EventType ret;
        ret.is_event_occurred = false;

        if(!data.contains(typeid(EventType))){
            return ret;
        }
        for(const std::any& a : data[typeid(EventType)]){
            ret = std::any_cast<EventType>(a);
            break;
        }
        return ret;
    }
    template<typename EventType>
    bool has(){
        DataContainer& data = (EventType::is_event_unique) ? m_events : m_events_buffer;

        if(!data.contains(typeid(EventType))){
            return false;
        }
        return true;
    }
    template<typename EventType>
    void block(){
        DataContainer& data = (EventType::is_event_unique) ? m_events : m_events_buffer;

        if(data.contains(typeid(EventType))){
            data.erase(typeid(EventType));
        }else{
            m_block_types.emplace(typeid(EventType));
        }
        return;
    }
    template<typename EventType>
    void emit(EventType p_event){
        if(EventType::is_event_unique && this->has<EventType>()){
            return;
        }

        if(m_block_types.contains(typeid(EventType))){
            return;
        }

        if(!m_events.contains(typeid(EventType))){
            m_events.emplace(typeid(EventType),std::vector<std::any>());
        }
        m_events[typeid(EventType)].push_back(p_event);
    }
    void flush();

    void emit_locked_all();
private:
    DataContainer m_events;
    DataContainer m_events_buffer;
    std::set<std::type_index> m_block_types;
};