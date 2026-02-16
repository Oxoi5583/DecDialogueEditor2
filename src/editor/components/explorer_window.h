#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/abstract/singleton.h"
#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"
#include <cstddef>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "DecToolsBox/core/random_code.h"

class ExplorerWindow : public ObjectBase{
private:
    void m_close_button_process();
    std::string m_uid = RandomCode(25).get();
public:
    ExplorerWindow();
    ~ExplorerWindow();

    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};
