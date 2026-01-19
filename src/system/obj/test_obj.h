#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "server/object_base.h"

class TestObj : public ObjectBase {
private:

public:
    TestObj();
    ~TestObj();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};