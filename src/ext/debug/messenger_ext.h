#pragma once

#include "DecToolsBox/container/ordered_list.h"
#include "glm/ext/vector_float2.hpp"

namespace DecToolsBox {

inline Messenger& operator<<(Messenger& msg, const glm::vec2& vec) {
    std::stringstream ss;
    ss << "(" << vec.x << ", " << vec.y << ")";
    msg << ss.str();
    return msg;
}

}