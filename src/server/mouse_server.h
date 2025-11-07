#pragma once

#include "DecToolsBox/abstract/singleton.h"
#include "glm/glm.hpp"
#include "engine/input_hub.h"
#include "SDL3/SDL_mouse.h"
#include <vector>

using namespace glm;

enum class MouseButton {
    LEFT,
    RIGHT,
    MIDDLE
};

class MouseServer : public Singleton<MouseServer> {
private:
    vec2 m_world_mouse_pos;
    vec2 m_screen_mouse_pos;
    vec2 m_screen_mouse_pos_center;

    bool m_is_mouse_in_window = false;

    bool m_is_left_just_clicked = false;
    bool m_is_left_clicked = false;
    bool m_is_left_just_released = false;

    bool m_is_right_just_clicked = false;
    bool m_is_right_clicked = false;
    bool m_is_right_just_released = false;

    bool m_is_middle_just_clicked = false;
    bool m_is_middle_clicked = false;
    bool m_is_middle_just_released = false;

    std::vector<SDL_Cursor*> cursors;

    // event cache
    std::vector<class EventMouseOnResizer> m_resizer_event;
    std::vector<class EventMouseHoverObj> m_obj_hover_event;

    void m_emit_event_if_left_just_clicked();
    void m_emit_event_if_left_clicked();
    void m_emit_event_if_left_released();

    void m_emit_event_if_right_just_clicked();
    void m_emit_event_if_right_clicked();
    void m_emit_event_if_right_released();

    void m_emit_event_if_middle_just_clicked();
    void m_emit_event_if_middle_clicked();
    void m_emit_event_if_middle_released();

    void m_set_cursor(int p_index);
    void m_event_handle_reset();
    void m_event_handle_resize_event();
    void m_event_handle_hover_event();

public:
    MouseServer();
    ~MouseServer();

    void update();
    void load_cursor();

    vec2 get_mouse_screen_position() const;
    vec2 get_mouse_screen_position_center() const;
    vec2 get_mouse_world_position() const;

    bool is_just_clicked(MouseButton p_button = MouseButton::LEFT);
    bool is_clicked(MouseButton p_button = MouseButton::LEFT);
    bool is_just_released(MouseButton p_button = MouseButton::LEFT);
    bool is_mouse_in_window();

    // Cursor control
    void cursor_default();
    void cursor_text();
    void cursor_wait();
    void cursor_crosshair();
    void cursor_progress();
    void cursor_pointer();
    void cursor_move();
    void cursor_not_allowed();
    void cursor_NS_resize();
    void cursor_EW_resize();
    void cursor_NWSE_resize();
    void cursor_NESW_resize();
    void cursor_NW_resize();
    void cursor_N_resize();
    void cursor_NE_resize();
    void cursor_E_resize();
    void cursor_SE_resize();
    void cursor_S_resize();
    void cursor_SW_resize();
    void cursor_W_resize();
};
