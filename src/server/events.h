#pragma once

#include "glm/ext/vector_float2.hpp"

using namespace glm;

struct EventBase{
    bool is_event_occurred = false;
};

struct EventMouseJustClickedOnWorld : public EventBase{
    vec2 click_pos;
};

struct EventMouseClickedOnWorld : public EventBase{
    vec2 click_pos;
};

struct EventMouseReleased : public EventBase{
    vec2 click_pos;
};

