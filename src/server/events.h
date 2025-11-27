#pragma once

#include "editor/layout.h"
#include "glm/ext/vector_float2.hpp"
#include "obj/graph/manager.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <vector>

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

struct EventMouseHoverObjLastFrame : public NonUniqueEvent{
    vec2 hovering_pos;
    OID obj_id;

    bool is_pointer_cursor = false;
};

struct EventMouseJustClickObj : public NonUniqueEvent{
    vec2 click_pos;
    OID obj_id;
    ObjectServer::Layer layer;
};

struct EventMouseClickObj : public NonUniqueEvent{
    vec2 click_pos;
    OID obj_id;
    ObjectServer::Layer layer;
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

struct EventMouseHoverOnWorld : public NonUniqueEvent{
    vec2 screen_pos;
    vec2 pos;
};

struct EventMouseJustClickedOnWorld : public NonUniqueEvent{
    vec2 screen_pos;
    vec2 pos;
    int button;
};

struct EventEditorSpaceResizerHover : public UniqueEvent{};
struct EventEditorSpaceResizerDragging : public UniqueEvent{};


struct EventSpawnNode : public NonUniqueEvent{
    vec2 spawn_pos;
    GraphManager::NodeType type;
};

struct EventMouseJustClickSelectedObj : public NonUniqueEvent{
    vec2 click_pos;
    OID obj_id;
};

struct EventMouseSelectedObj : public NonUniqueEvent{
    OID obj_id;
};

struct EventSelectedObjDragging : public NonUniqueEvent{
    OID obj_id;
};

struct EventSelectedObjHovering : public NonUniqueEvent{
    OID obj_id;
};


struct EventSelectedObjPlaced : public NonUniqueEvent{
    OID obj_id;
};

struct EventSelectedObjGroupDragging : public UniqueEvent{};
struct EventSelectedObjGroupPlaced : public UniqueEvent{};

struct EventLeftPanelItemHovered : public NonUniqueEvent{
    OID id;
};
struct EventLeftPanelSelectedItemHovered : public NonUniqueEvent{
    std::vector<OID> ids;
};

struct EventLeftPanelHovered : public NonUniqueEvent{};

struct EventStartConnect : public NonUniqueEvent{
    OID id;
};
struct EventSearchingConnect : public NonUniqueEvent{
    OID id;
};
struct EventCreateConnection : public NonUniqueEvent{
    OID fm_id;
    OID to_id;
};
struct EventRemoveConnection : public NonUniqueEvent{
    OID fm_id;
    OID to_id;
};
struct EventCreateConnectionLinePart : public NonUniqueEvent{
    OID parent_id;
    OID id;
};
struct EventRemoveConnectionLinePart : public NonUniqueEvent{
    OID parent_id;
    OID id;
};


struct EventLockedAll : public NonUniqueEvent{};
