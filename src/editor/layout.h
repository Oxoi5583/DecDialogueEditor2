#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/container/ordered_list.h"
#include "editor/space.h"
#include "glm/ext/scalar_int_sized.hpp"
#include "glm/ext/scalar_uint_sized.hpp"
#include "glm/ext/vector_float2.hpp"
#include "struct/shape/rect2.h"
#include <map>
#include <memory>
#include <vector>

using namespace glm;

class EditorLayout : public Singleton<EditorLayout>{
private:
    vec2 m_window_size_buffer;
    EditorSpace m_space;
public:
    void init();
    void update();
};