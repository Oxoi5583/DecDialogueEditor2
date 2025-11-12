#pragma once

#include "editor/layout.h"
#include "glm/ext/vector_float2.hpp"
#include "server/mouse_server.h"
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
    int button;
};

struct EventMouseClicked : public UniqueEvent{
    vec2 click_pos;
    int button;
};

struct EventMouseReleased : public UniqueEvent{
    vec2 click_pos;
    int button;
};


struct EventMouseHoverObj : public UniqueEvent{
    vec2 hovering_pos;
    OID obj_id;

    bool is_pointer_cursor = false;
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

struct EventWindowResized : public UniqueEvent{
    vec2 new_size;
};

enum class EventDirection{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    DOWN_LEFT,
    UP_RIGHT,
    DOWN_RIGHT,
};

struct EventMouseOnResizer : public UniqueEvent{
    EventDirection dir;
};

struct EventDragResizer : public NonUniqueEvent{
    EventDirection dir;
    vec2 global_mouse_pos;
};

struct EventMouseJustClickedOnWorld : public NonUniqueEvent{
    vec2 pos;
    int button;
};

struct EventEditorSpaceResizerHover : public UniqueEvent{};
struct EventEditorSpaceResizerDragging : public UniqueEvent{};



enum NodeType{
    ENTRY,
    NODE,
    OPTION,
};


struct EventSpawnNode : public NonUniqueEvent{
    vec2 spawn_pos;
    NodeType type;
};