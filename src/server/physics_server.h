#pragma once

#include <DecToolsBox/abstract/singleton.h>
#include "glm/ext/vector_float2.hpp"
#include <array>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "boost/unordered_map.hpp"
#include "boost/unordered_set.hpp"

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
    std::array<glm::vec2, 4> get_points();
};

struct SpaceIndexes{
    const glm::vec2 m_cell = {250, 250};

    boost::unordered_map<ShapeId, boost::unordered_set<CellId>> shape_to_cells;
    boost::unordered_map<CellId, boost::unordered_set<ShapeId>> cell_to_shape;
    const boost::unordered_set<CellId> empty_cells;
    const boost::unordered_set<ShapeId> empty_shapes;
    
    void set_index(const ShapeId& p_sid, const CellId& p_cid);
    void erase_index(const ShapeId& p_sid);

    const boost::unordered_set<ShapeId>& get_shapes(const CellId& p_cid);
    const boost::unordered_set<CellId>& get_cells(const ShapeId& p_sid);

    bool is_shape_in_same_chunk(const ShapeId& p_sid_1, const ShapeId& p_sid_2);
    bool is_shape_in_same_chunk(RawRect p_rect, const ShapeId& p_sid);
    bool is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, const ShapeId& p_sid);


    boost::unordered_map<int, boost::unordered_map<int, CellId>> cell_id_cache;
    bool is_cache_exists(int p_x, int p_y);
    const CellId& get_cell_id(int p_x, int p_y);

    CellId pos_to_cell_id(glm::vec2 p_pos);
    boost::unordered_set<CellId> rect_to_cell_ids(RawRect p_rect);
};

struct InstanceMap{
    boost::unordered_map<ShapeId, RawRect> m_shapes_data;

    void set_instance(const ShapeId& p_sid, RawRect p_rect);
    void erase_instance(const ShapeId& p_sid);

    RawRect get_shape(const ShapeId& p_sid);
};

struct PendPackage{
    ShapeId shape_id;
    RawRect raw_rect;
};

class PhysicsServer : public Singleton<PhysicsServer>{
private:
    InstanceMap m_instace_map;
    SpaceIndexes m_space_indexes;

    ShapeId m_next_shape_id = 1000;

    std::queue<PendPackage> m_pending_packages;
public:
    void update();

    void set_instance(const ShapeId& p_sid, RawRect p_rect);
    void erase_instance(const ShapeId& p_sid);

    bool is_shape_in_same_chunk(const ShapeId& p_sid_1, const ShapeId& p_sid_2);
    bool is_shape_in_same_chunk(RawRect p_rect, const ShapeId& p_sid);
    bool is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2);
    bool is_shape_in_same_chunk(glm::vec2 p_pos, const ShapeId& p_sid);

    ShapeId next_shape_id();
};