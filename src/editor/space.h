#pragma once

#include "glm/ext/vector_float2.hpp"
#include "core/timer_server.h"
#include "struct/shape/rect2.h"
#include <array>
#include <cstddef>
#include <queue>
#include <vector>

using namespace glm;

class EditorSpace : public Rect2 {
public:
    enum class From {
        START,
        END,
    };

    From from = From::START;

    struct SplitLimit {
        enum class Type {
            PROPORTION,
            VALUE,
        };

        double min = std::numeric_limits<double>::min();
        double max = std::numeric_limits<double>::max();
        Type type = Type::PROPORTION;

        void enable();
        void disable();
        bool is_enabled();

        // --- Copy & Move ---
        SplitLimit() = default;
        SplitLimit(const SplitLimit& other);
        SplitLimit& operator=(const SplitLimit& other);
        SplitLimit(SplitLimit&& other) noexcept;
        SplitLimit& operator=(SplitLimit&& other) noexcept;

    private:
        bool m_is_enabled = false;
    };

    struct SplitFixed {
        enum class Type {
            PROPORTION,
            VALUE,
        };

        double value = std::numeric_limits<double>::max();
        Type type = Type::PROPORTION;

        void enable();
        void disable();
        bool is_enabled();

        // --- Copy & Move ---
        SplitFixed() = default;
        SplitFixed(const SplitFixed& other);
        SplitFixed& operator=(const SplitFixed& other);
        SplitFixed(SplitFixed&& other) noexcept;
        SplitFixed& operator=(SplitFixed&& other) noexcept;

    private:
        bool m_is_enabled = false;
    };

    struct SplitResizer {
        enum class State {
            IDLE,
            HOVER,
            DRAGGING,
        };

        double resizer_size = 10.0f;

        void enable();
        void disable();
        bool is_enabled();

        // --- Copy & Move ---
        SplitResizer() = default;
        SplitResizer(const SplitResizer& other);
        SplitResizer& operator=(const SplitResizer& other);
        SplitResizer(SplitResizer&& other) noexcept;
        SplitResizer& operator=(SplitResizer&& other) noexcept;

    private:
        bool m_is_enabled = false;
        State state = State::IDLE;
        void m_trans_state_event_to_IDLE();
        void m_trans_state_event_to_HOVER();
        void m_trans_state_event_to_DRAGGING();
        void m_state_event_in_IDLE();
        void m_state_event_in_HOVER();
        void m_state_event_in_DRAGGING();

        State m_go_to_state(State p_state);
        void m_emit_event();

        friend class EditorSpace;
    };


    SplitLimit split_limit;
    SplitFixed split_fixed;
    SplitResizer split_resizer;

    void update_resizer();
    bool is_resizing();

    struct Children {
        EditorSpace* first = nullptr;
        EditorSpace* second = nullptr;
    };

    enum class SplitType {
        VERTICLE,
        HORIZONTAL,
    };

    Rect2 get_resizer_area();

    void unsplit();
    void split();
    void split(double m_proportion);

    void refresh_children();

    std::vector<EditorSpace> get_spaces() const;
    std::vector<EditorSpace*> get_spaces_ptr();
    std::array<Rect2, 2> get_rects() const;
    double get_proportion(vec2 p_pos) const;

    EditorSpace* get_parent();
    void set_parent(EditorSpace* p_space);
    void set_type(SplitType p_type);
    Children get_children();

    EditorSpace();
    ~EditorSpace();

    EditorSpace(SplitType p_type, vec2 p_pos, vec2 p_size);
    EditorSpace(SplitType p_type, vec2 p_pos, vec2 p_size, double p_proportion);

    // --- Copy & Move ---
    EditorSpace(const EditorSpace& other);
    EditorSpace& operator=(const EditorSpace& other);
    EditorSpace(EditorSpace&& other) noexcept;
    EditorSpace& operator=(EditorSpace&& other) noexcept;

private:
    double m_split = 0.0f;
    double m_get_fixed_split();
    double m_get_limited_split(double p_value);

    SplitType m_type = SplitType::VERTICLE;
    std::array<Rect2, 2> m_get_rects_verticle() const;
    std::array<Rect2, 2> m_get_rects_horizontal() const;

    double m_get_proportion_verticle(vec2 p_pos) const;
    double m_get_proportion_horizontal(vec2 p_pos) const;

    std::vector<EditorSpace> m_spaces;
    EditorSpace* m_parent = nullptr;
    void m_try_create_children();
};
