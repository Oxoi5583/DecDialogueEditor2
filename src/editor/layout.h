#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/container/ordered_list.h"
#include "glm/ext/scalar_uint_sized.hpp"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"
#include <map>
#include <memory>
#include <vector>

using namespace glm;

typedef uint32 LID;

class EditorSpaceBase;

class EditorLayout : Singleton<EditorLayout>{
private:
    std::unique_ptr<EditorSpaceBase> m_main_layer = std::make_unique<EditorSpaceBase>();
public:
    void init();
    void update();
};

