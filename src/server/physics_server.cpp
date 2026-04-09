#include "server/physics_server.h"
#include "physics_server.h"
#include <DecToolsBox/debug/messenger.h>
#include <cmath>
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <vector>

RawRect::RawRect(const glm::vec2& p_min,const glm::vec2& p_max)
: min(glm::min(p_min, p_max))
, max(glm::max(p_min, p_max)){}
glm::vec2 RawRect::get_left_top(){ return min; };
glm::vec2 RawRect::get_right_top(){ return {max.x, min.y}; };
glm::vec2 RawRect::get_left_down(){ return {min.x, max.y}; };
glm::vec2 RawRect::get_right_down(){ return max; };

std::vector<glm::vec2> RawRect::get_points(){
    return {get_left_top(), get_right_top(), get_right_down(), get_left_down()};
}


void PhysicsServer::set_instance(ShapeId p_sid, RawRect p_rect){
    m_instace_map.set_instance(p_sid, p_rect);
}
void PhysicsServer::erase_instance(ShapeId p_sid){
    m_space_indexes.erase_index(p_sid);
    m_instace_map.erase_instance(p_sid);
}

CellId SpaceIndexes::pos_to_cell_id(glm::vec2 p_pos){
    int in_x = std::floor(p_pos.x / m_cell.x);
    int in_y = std::floor(p_pos.y / m_cell.y);

    return std::to_string(in_x) + ":" + std::to_string(in_y);
}
std::unordered_set<CellId> SpaceIndexes::rect_to_cell_ids(RawRect p_rect){
    glm::vec2 min = p_rect.min;
    glm::vec2 max = p_rect.max;

    int min_in_x = std::floor(min.x / m_cell.x);
    int min_in_y = std::floor(min.y / m_cell.y);
    int max_in_x = std::floor(max.x / m_cell.x);
    int max_in_y = std::floor(max.y / m_cell.y);


    std::unordered_set<CellId> cells;
    for(int x = min_in_x; x < max_in_x + 1; x++){
        for(int y = min_in_y; y < max_in_y + 1; y++){
            std::string cell_id = std::to_string(x) + ":" + std::to_string(y);
            cells.emplace(cell_id);
        }
    }
    return cells;
}

void PhysicsServer::update(){
    for(auto it : m_instace_map.m_shapes_data){
        ShapeId sid = it.first;
        RawRect rect = it.second;

        std::unordered_set<CellId> cells = m_space_indexes.rect_to_cell_ids(rect);

        m_space_indexes.erase_index(sid);
        for(CellId cell : cells){
            m_space_indexes.set_index(sid, cell);
        }
    }
}

void InstanceMap::set_instance(ShapeId p_sid, RawRect p_rect){
    if(m_shapes_data.contains(p_sid)){
        m_shapes_data[p_sid] = p_rect;
    }else{
        m_shapes_data.try_emplace(p_sid, p_rect);
    }
}
void InstanceMap::erase_instance(ShapeId p_sid){
    if(m_shapes_data.contains(p_sid)){
        m_shapes_data.erase(p_sid);
    }
}
RawRect InstanceMap::get_shape(ShapeId p_sid){
    if(m_shapes_data.contains(p_sid)){
        return m_shapes_data[p_sid];
    }
    
    return {};
}


void SpaceIndexes::set_index(ShapeId p_sid, CellId p_cid){
    if(!shape_to_cells.contains(p_sid)){
        shape_to_cells.emplace(p_sid, std::unordered_set<CellId>());
    }

    if(!shape_to_cells[p_sid].contains(p_cid)){
        shape_to_cells[p_sid].insert(p_cid);
    }

    if(!cell_to_shape.contains(p_cid)){
        cell_to_shape.emplace(p_cid, std::unordered_set<ShapeId>());
    }

    if(!cell_to_shape[p_cid].contains(p_sid)){
        cell_to_shape[p_cid].insert(p_sid);
    }
}
void SpaceIndexes::erase_index(ShapeId p_sid){
    std::unordered_set<CellId> cells;
    if(shape_to_cells.contains(p_sid)){
        cells = shape_to_cells[p_sid];
        shape_to_cells.erase(p_sid);
    }

    for(CellId cell : cells){
        if(cell_to_shape.contains(cell)){
            cell_to_shape[cell].erase(p_sid);
        }
    }
}
const std::unordered_set<ShapeId>& SpaceIndexes::get_shapes(CellId p_cid){
    if(!cell_to_shape.contains(p_cid)){
        return empty_shapes;
    }
    return cell_to_shape[p_cid];
}
const std::unordered_set<CellId>& SpaceIndexes::get_cells(ShapeId p_sid){
    if(!shape_to_cells.contains(p_sid)){
        return empty_cells;
    }
    return shape_to_cells[p_sid];
}

bool SpaceIndexes::is_shape_in_same_chunk(ShapeId p_sid_1, ShapeId p_sid_2){
    const std::unordered_set<CellId>& cells_for_1 = get_cells(p_sid_1);
    const std::unordered_set<CellId>& cells_for_2 = get_cells(p_sid_2);
    for(CellId cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}
bool SpaceIndexes::is_shape_in_same_chunk(RawRect p_rect, ShapeId p_sid){
    std::unordered_set<CellId> cells_for_1 = rect_to_cell_ids(p_rect);
    const std::unordered_set<CellId>& cells_for_2 = get_cells(p_sid);
    for(CellId cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;   
}
bool SpaceIndexes::is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2){
    std::unordered_set<CellId> cells_for_1 = rect_to_cell_ids(p_rect_1);
    std::unordered_set<CellId> cells_for_2 = rect_to_cell_ids(p_rect_2);
    for(CellId cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}
bool SpaceIndexes::is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2){
    std::unordered_set<CellId> cells_for_1 = {pos_to_cell_id(p_pos)};
    std::unordered_set<CellId> cells_for_2 = rect_to_cell_ids(p_rect_2);
    for(CellId cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}
bool SpaceIndexes::is_shape_in_same_chunk(glm::vec2 p_pos, ShapeId p_sid){
    std::unordered_set<CellId> cells_for_1 = {pos_to_cell_id(p_pos)};
    const std::unordered_set<CellId>& cells_for_2 = get_cells(p_sid);
    for(CellId cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}


bool PhysicsServer::is_shape_in_same_chunk(ShapeId p_sid_1, ShapeId p_sid_2){
    return m_space_indexes.is_shape_in_same_chunk(p_sid_1, p_sid_2);
}
bool PhysicsServer::is_shape_in_same_chunk(RawRect p_rect, ShapeId p_sid){
    return m_space_indexes.is_shape_in_same_chunk(p_rect, p_sid);
}
bool PhysicsServer::is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2){
    return m_space_indexes.is_shape_in_same_chunk(p_rect_1, p_rect_2);
}
bool PhysicsServer::is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2){
    return m_space_indexes.is_shape_in_same_chunk(p_pos, p_rect_2);
}
bool PhysicsServer::is_shape_in_same_chunk(glm::vec2 p_pos, ShapeId p_sid){
    return m_space_indexes.is_shape_in_same_chunk(p_pos, p_sid);
}

ShapeId PhysicsServer::next_shape_id(){
    m_next_shape_id++;
    return m_next_shape_id;
}