#pragma once

#include "glm/ext/vector_float2.hpp"
#include "core/timer_server.h"
#include "struct/shape/rect2.h"
#include <array>
#include <cstddef>
#include <queue>
#include <vector>

using namespace glm;

class EditorSpace : public Rect2{
public:
    enum class From{
        START,
        END,
    };

    From from = From::START;

    struct SplitLimit{
        enum class Type{
            PROPORTION,
            VALUE,
        };

        double min;
        double max;

        Type type = Type::PROPORTION;
        bool is_enabled = false;

        void enable();
        void disable();
    };
    struct SplitFixed{
        enum class Type{
            PROPORTION,
            VALUE,
        };

        double value;

        Type type = Type::PROPORTION;
        bool is_enabled = false;

        void enable();
        void disable();
    };

    SplitLimit split_limit = {std::numeric_limits<double>::min(), std::numeric_limits<double>::max()};
    SplitFixed split_fixed = {std::numeric_limits<double>::max()};

    struct Children{
        EditorSpace* first = nullptr;
        EditorSpace* second = nullptr;
    };

    enum class SplitType{
        VERTICLE,
        HORIZONTAL,
    };

    void unsplit();
    void split();
    void split(double m_proportion);

    void refresh_children();

    std::vector<EditorSpace> get_spaces() const;
    std::array<Rect2,2> get_rects() const;
    double get_proportion(vec2 p_pos) const;
    
    

    void set_type(SplitType p_type);

    Rect2 to_screen();

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

    std::array<Rect2,2> m_get_rects_verticle() const;
    std::array<Rect2,2> m_get_rects_horizontal() const;

    double m_get_proportion_verticle(vec2 p_pos) const;
    double m_get_proportion_horizontal(vec2 p_pos) const;

    std::vector<EditorSpace> m_spaces;

};