#pragma once

#include "DecToolsBox/debug/messenger.h"

template<typename T>
class Singleton{
public:
    static T* Ref(){
        static T instance;
        return &instance;
    }
};
