#pragma once

#include "glm/ext/vector_float2.hpp"
#include "server/object_base.h"

using namespace glm;

struct EventBase{
    bool is_event_occurred = true;
};

struct UniqueEvent : public EventBase{
    static constexpr bool is_event_unique = true;
};

struct NonUniqueEvent : public EventBase{
    static constexpr bool is_event_unique = false;
};

struct EventMouseJustClicked : public UniqueEvent{
    vec2 click_pos;
};

struct EventMouseClicked : public UniqueEvent{
    vec2 click_pos;
};

struct EventMouseReleased : public UniqueEvent{
    vec2 click_pos;
};

struct EventSpawnNode : public NonUniqueEvent{
    vec2 spawn_pos;
};

struct EventMouseHoverObj : public UniqueEvent{
    vec2 hovering_pos;
    OID obj_id;
};

struct EventMouseJustClickObj : public UniqueEvent{
    vec2 click_pos;
    OID obj_id;
};

struct EventMouseClickObj : public UniqueEvent{
    vec2 click_pos;
    OID obj_id;
};

struct EventMouseDragObj : public NonUniqueEvent{
    vec2 drag_pos;
    OID obj_id;
};

struct EventMousePlaceObj : public NonUniqueEvent{
    vec2 place_pos;
    OID obj_id;
};