#pragma once

#include "glm/ext/vector_float2.hpp"

using namespace glm;

struct EventMouseJustClickedOnWorld{
    vec2 click_pos;
};

struct EventMouseClickedOnWorld{
    vec2 click_pos;
};

struct EventMouseReleased{
    vec2 click_pos;
};

