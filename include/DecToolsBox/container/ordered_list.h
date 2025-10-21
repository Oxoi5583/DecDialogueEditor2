#pragma once

#include "DecToolsBox/debug/messenger.h"
#include <vector>
#include <map>
#include <iostream>
#include <assert.h>

template <typename T>
class ordered_list{
private:
    enum class ElementType{
        ET_FIRST,
        ET_MIDDLE,
        ET_LAST,
        ET_ONLY,
        ET_NULL,
    };
    struct Arrow{
        ElementType type = ElementType::ET_NULL;
        T* front = nullptr;
        T* self = nullptr;
        T* back = nullptr;
        Arrow(T* f = nullptr, T* s = nullptr, T* b = nullptr){
            front = f;
            self = s;
            back = b;
        }
    };
    std::map<T*, Arrow> storage;
    T* first = nullptr;
    T* last = nullptr;

    inline ElementType m_check_element_type(T* p_item){
        if(storage.contains(p_item)){
            T* item_front = storage[p_item].front;
            T* item_back = storage[p_item].back;
            if(item_front == nullptr && item_back == nullptr){
                return ElementType::ET_ONLY;
            }
            if(item_front == nullptr && item_back != nullptr){
                return ElementType::ET_FIRST;
            }
            if(item_front != nullptr && item_back == nullptr){
                return ElementType::ET_LAST;
            }
            if(item_front != nullptr && item_back != nullptr){
                return ElementType::ET_MIDDLE;
            }
        }
        return ElementType::ET_NULL;
    }

    inline ElementType m_update_arrow_type(T* item){
        if(storage.contains(item)){
            ElementType ret = m_check_element_type(item);
            storage[item].type = ret;
            return ret;
        }else{
            return ElementType::ET_NULL;
        }
    }

    inline void m_extract_element_temp(T* item){
        ElementType type = storage[item].type;

        if(type == ElementType::ET_NULL || type == ElementType::ET_ONLY){
            return;
        }

        T* item_front = storage[item].front;
        T* item_back = storage[item].back;

        switch(type){
            case ElementType::ET_FIRST: {
                storage[item_back].front = nullptr;
                first = item_back;
                break;
            }
            case ElementType::ET_LAST: {
                storage[item_front].back = nullptr;
                last = item_front;
                break;
            }
            case ElementType::ET_MIDDLE: {
                storage[item_front].back = item_back;
                storage[item_back].front = item_front;
                break;
            }
            default: {
                break;
            }
        }

        storage[item].front = nullptr;
        storage[item].back = nullptr;
    }
    inline void m_place_element_to_first(T* item){
        storage[item].front = nullptr;
        storage[item].back = first;
        storage[first].front = item;
        first = item;
    }
    inline void m_place_element_to_last(T* item){
        storage[last].back = item;
        storage[item].front = last;
        storage[item].back = nullptr;
        last = item;
    }
public:
    inline void move_to_front(T* item){
        ElementType type = m_update_arrow_type(item);
        if(type == ElementType::ET_NULL || type == ElementType::ET_ONLY) return;
        
        m_extract_element_temp(item);
        m_place_element_to_first(item);
    }
    inline void move_to_back(T* item){
        ElementType type = m_update_arrow_type(item);
        if(type == ElementType::ET_NULL || type == ElementType::ET_ONLY) return;
        
        m_extract_element_temp(item);
        m_place_element_to_last(item);
    }
    inline void move_to_specific_back(T* item, T* target){
        ElementType type = m_update_arrow_type(item);
        if(type == ElementType::ET_NULL || type == ElementType::ET_ONLY) return;
        
        ElementType target_type = m_update_arrow_type(target);
        if(target_type == ElementType::ET_NULL) return;
        
        m_extract_element_temp(item);

        switch(target_type){
            case ElementType::ET_FIRST: {
                T* prev = target;
                T* next = storage[target].back;
                storage[prev].back = item;
                storage[next].front = item;

                storage[item].front = prev;
                storage[item].back = next;
                break;
            }
            case ElementType::ET_LAST: {
                this->m_place_element_to_last(item);
                break;
            }
            case ElementType::ET_MIDDLE: {
                T* prev = target;
                T* next = storage[target].back;
                storage[prev].back = item;
                storage[next].front = item;
                
                storage[item].front = prev;
                storage[item].back = next;
                break;
            }
            default: {
                break;
            }
        }
    }
    inline void move_to_specific_front(T* item, T* target){
        ElementType type = m_update_arrow_type(item);
        if(type == ElementType::ET_NULL || type == ElementType::ET_ONLY) return;
        
        ElementType target_type = m_update_arrow_type(target);
        if(target_type == ElementType::ET_NULL) return;
        
        m_extract_element_temp(item);

        switch(target_type){
            case ElementType::ET_FIRST: {
                this->m_place_element_to_first(item);
                break;
            }
            case ElementType::ET_LAST: {
                T* prev = storage[target].front;
                T* next = target;
                storage[prev].back = item;
                storage[next].front = item;

                storage[item].front = prev;
                storage[item].back = next;
                break;
            }
            case ElementType::ET_MIDDLE: {
                T* prev = storage[target].front;
                T* next = target;
                storage[prev].back = item;
                storage[next].front = item;

                storage[item].front = prev;
                storage[item].back = next;
                break;
            }
            default: {
                break;
            }
        }
    }

    inline void push_back(T* item){
        assert(item != nullptr);
        if(last == nullptr){
            first = item;
            last = item;
            storage.emplace(item, Arrow(nullptr, item, nullptr));
        }else{
            storage.emplace(item, Arrow(last, item, nullptr));
            storage[last].back = item;
            last = item;
        }
    }

    inline void push_front(T* item){
        assert(item != nullptr);
        if(first == nullptr){
            first = item;
            last = item;
            storage.emplace(item, Arrow(nullptr, item, nullptr));
        }else{
            storage.emplace(item, Arrow(nullptr, item, first));
            storage[first].front = item;
            first = item;
        }
    }

    void erase(T* p_ptr){
        if(storage.contains(p_ptr)){
            T* d_front = storage[p_ptr].front;
            T* d_back = storage[p_ptr].back;
            if(d_front == nullptr && d_back == nullptr){
                first = nullptr;
                last = nullptr;
            }
            if(d_front == nullptr && d_back != nullptr){
                storage[d_back].front = nullptr;
                first = d_back;
            }
            if(d_front != nullptr && d_back == nullptr){
                storage[d_front].back = nullptr;
                last = d_front;
            }
            if(d_front != nullptr && d_back != nullptr){
                storage[d_front].back = d_back;
                storage[d_back].front = d_front;
            }
            storage.erase(p_ptr);
        }
    }

    class Iterator {
    private:
        T* current;
        std::map<T*, Arrow>* storage;

    public:
        Iterator(T* start, std::map<T*, Arrow>* storage) : current(start), storage(storage) {}

        inline T* operator*() { return current; }
        inline Iterator& operator++() {
            auto it = storage->find(current);
            if (it != storage->end()) {
                current = it->second.back;
            } else {
                current = nullptr;
            }
            return *this;
        }
        inline Iterator& operator++(int) {
            auto it = storage->find(current);
            if (it != storage->end()) {
                current = it->second.back;
            } else {
                current = nullptr;
            }
            return *this;
        }
        inline bool operator!=(const Iterator& other) const { return current != other.current; }
        inline T* operator->() const { return current; }
    };

    Iterator begin() { return Iterator(first, &storage); }
    Iterator end() { return Iterator(nullptr, &storage); }

    
    class ReverseIterator {
    private:
        T* current;
        std::map<T*, Arrow>* storage;

    public:
        ReverseIterator(T* start, std::map<T*, Arrow>* storage) : current(start), storage(storage) {}

        inline T* operator*() { return current; }
        inline ReverseIterator& operator++() {
            auto it = storage->find(current);
            if (it != storage->begin()) {
                current = it->second.front;
            } else {
                current = nullptr;
            }
            return *this;
        }
        inline ReverseIterator& operator++(int) {
            auto it = storage->find(current);
            if (it != storage->begin()) {
                current = it->second.front;
            } else {
                current = nullptr;
            }
            return *this;
        }
        inline bool operator!=(const ReverseIterator& other) const { return current != other.current; }
        inline T* operator->() const { return current; }
    };

    ReverseIterator rev_begin() { return ReverseIterator(last, &storage); }
    ReverseIterator rev_end() { return ReverseIterator(nullptr, &storage); }

    inline std::vector<T*> get_vector(){
        std::vector<T*> ret;
        for(auto it = this->begin(); it != this->end(); ++it){
            ret.push_back(*it);
        }
        return ret;
    }

    inline int size(){
        int ret = 0;
        for(T* it : *this){
            ret++;
        }
        return ret;
    }
};

/*
Note :
Add Reverse Iterator

*/
