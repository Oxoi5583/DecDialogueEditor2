#pragma once

#include "DecToolsBox/debug/messenger.h"

class TestBase{
public:
    virtual void print(){
        DEBUG_MSG("BASE");
    }
    inline TestBase* operator->(){
        return this;
    }
};