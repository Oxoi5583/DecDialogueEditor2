#pragma once

#include "DecToolsBox/container/ordered_list.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

namespace DecToolsBox {

inline Messenger& operator<<(Messenger& msg, const glm::vec2& vec) {
    std::stringstream ss;
    ss << "(" << vec.x << ", " << vec.y << ")";
    msg << ss.str();
    return msg;
}
inline Messenger& operator<<(Messenger& msg, const glm::vec3& vec) {
    std::stringstream ss;
    ss << "(" << vec.x << ", " << vec.y << ", " << vec.z  << ")";
    msg << ss.str();
    return msg;
}

}