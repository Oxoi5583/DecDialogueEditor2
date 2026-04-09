#pragma once

#include <DecToolsBox/abstract/singleton.h>
#include "glm/ext/vector_float2.hpp"
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

typedef unsigned long long ShapeId;
typedef std::string CellId;

struct RawRect{
    RawRect() = default;
    RawRect(const glm::vec2& p_min,const glm::vec2& p_max);

    glm::vec2 min;
    glm::vec2 max;

    glm::vec2 get_left_top();
    glm::vec2 get_right_top();
    glm::vec2 get_left_down();
    glm::vec2 get_right_down();
    std::vector<glm::vec2> get_points();
};

struct SpaceIndexes{
    const glm::vec2 m_cell = {250, 250};

    std::unordered_map<ShapeId, std::unordered_set<CellId>> shape_to_cells;
    std::unordered_map<CellId, std::unordered_set<ShapeId>> cell_to_shape;
    const std::unordered_set<CellId> empty_cells;
    const std::unordered_set<ShapeId> empty_shapes;
    
    void set_index(ShapeId p_sid, CellId p_cid);
    void erase_index(ShapeId p_sid);

    const std::unordered_set<ShapeId>& get_shapes(CellId p_cid);
    const std::unordered_set<CellId>& get_cells(ShapeId p_sid);

    bool is_shape_in_same_chunk(ShapeId p_sid_1, ShapeId p_sid_2);
    bool is_shape_in_same_chunk(RawRect p_rect, ShapeId p_sid);
    bool is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, ShapeId p_sid);

    CellId pos_to_cell_id(glm::vec2 p_pos);
    std::unordered_set<CellId> rect_to_cell_ids(RawRect p_rect);
};

struct InstanceMap{
    std::unordered_map<ShapeId, RawRect> m_shapes_data;

    void set_instance(ShapeId p_sid, RawRect p_rect);
    void erase_instance(ShapeId p_sid);

    RawRect get_shape(ShapeId p_sid);
};

class PhysicsServer : public Singleton<PhysicsServer>{
private:
    InstanceMap m_instace_map;
    SpaceIndexes m_space_indexes;

    ShapeId m_next_shape_id = 1000;
public:
    void update();

    void set_instance(ShapeId p_sid, RawRect p_rect);
    void erase_instance(ShapeId p_sid);

    bool is_shape_in_same_chunk(ShapeId p_sid_1, ShapeId p_sid_2);
    bool is_shape_in_same_chunk(RawRect p_rect, ShapeId p_sid);
    bool is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, ShapeId p_sid);

    ShapeId next_shape_id();
};