#pragma once

#include "DecToolsBox/debug/messenger.h"
#include "test/test_base.h"

class TestDerived : public TestBase{
public:
    void print() override{
        DEBUG_MSG("DERIVED");
    }
    
};