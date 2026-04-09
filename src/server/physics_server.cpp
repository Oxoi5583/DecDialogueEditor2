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

std::array<glm::vec2, 4> RawRect::get_points(){
    return {get_left_top(), get_right_top(), get_right_down(), get_left_down()};
}


void PhysicsServer::set_instance(const ShapeId& p_sid, RawRect p_rect){
    m_pending_packages.emplace(p_sid, p_rect);
}
void PhysicsServer::erase_instance(const ShapeId& p_sid){
    m_space_indexes.erase_index(p_sid);
    m_instace_map.erase_instance(p_sid);
}

bool SpaceIndexes::is_cache_exists(int p_x, int p_y){
    if(!cell_id_cache.contains(p_x)) return false;
    if(!cell_id_cache[p_x].contains(p_y)) return false;
    return true;
}
const CellId& SpaceIndexes::get_cell_id(int p_x, int p_y){
    if(is_cache_exists(p_x, p_y)){
        return cell_id_cache[p_x][p_y];
    }

    CellId cell_id = std::to_string(p_x) + ":" + std::to_string(p_y);
    if(!cell_id_cache.contains(p_x)) cell_id_cache.try_emplace(p_x, boost::unordered_map<int, CellId>());
    if(!cell_id_cache[p_x].contains(p_y)) cell_id_cache[p_x].try_emplace(p_y, cell_id);

    return cell_id_cache[p_x][p_y];
}

CellId SpaceIndexes::pos_to_cell_id(glm::vec2 p_pos){
    int in_x = std::floor(p_pos.x / m_cell.x);
    int in_y = std::floor(p_pos.y / m_cell.y);
    
    return get_cell_id(in_x, in_y);
}
boost::unordered_set<CellId> SpaceIndexes::rect_to_cell_ids(RawRect p_rect){
    glm::vec2 min = p_rect.min;
    glm::vec2 max = p_rect.max;

    int min_in_x = std::floor(min.x / m_cell.x);
    int min_in_y = std::floor(min.y / m_cell.y);
    int max_in_x = std::floor(max.x / m_cell.x);
    int max_in_y = std::floor(max.y / m_cell.y);

    boost::unordered_set<CellId> cells;
    for(int x = min_in_x; x < max_in_x + 1; x++){
        for(int y = min_in_y; y < max_in_y + 1; y++){
            cells.emplace(get_cell_id(x, y));
        }
    }
    return cells;
}

void PhysicsServer::update(){
    while(!m_pending_packages.empty()){
        PendPackage& package = m_pending_packages.front();

        ShapeId& sid = package.shape_id;
        RawRect& rect = package.raw_rect;
        
        m_instace_map.set_instance(sid, rect);
        
        boost::unordered_set<CellId> cells = m_space_indexes.rect_to_cell_ids(rect);
        m_space_indexes.erase_index(sid);
        for(auto& cell : cells){
            m_space_indexes.set_index(sid, cell);
        }

        m_pending_packages.pop();
    }
}

void InstanceMap::set_instance(const ShapeId& p_sid, RawRect p_rect){
    if(m_shapes_data.contains(p_sid)){
        m_shapes_data[p_sid] = p_rect;
    }else{
        m_shapes_data.try_emplace(p_sid, p_rect);
    }
}
void InstanceMap::erase_instance(const ShapeId& p_sid){
    if(m_shapes_data.contains(p_sid)){
        m_shapes_data.erase(p_sid);
    }
}
RawRect InstanceMap::get_shape(const ShapeId& p_sid){
    if(m_shapes_data.contains(p_sid)){
        return m_shapes_data[p_sid];
    }
    
    return {};
}


void SpaceIndexes::set_index(const ShapeId& p_sid, const CellId& p_cid){
    if(!shape_to_cells.contains(p_sid)){
        shape_to_cells.emplace(p_sid, boost::unordered_set<CellId>());
    }

    if(!shape_to_cells[p_sid].contains(p_cid)){
        shape_to_cells[p_sid].emplace(p_cid);
    }

    if(!cell_to_shape.contains(p_cid)){
        cell_to_shape.emplace(p_cid, boost::unordered_set<ShapeId>());
    }

    if(!cell_to_shape[p_cid].contains(p_sid)){
        cell_to_shape[p_cid].emplace(p_sid);
    }
}
void SpaceIndexes::erase_index(const ShapeId& p_sid){
    if(!shape_to_cells.contains(p_sid)){
        return;
    }

    boost::unordered_set<CellId>& cells = shape_to_cells[p_sid];
    
    for(auto& cell : cells){
        if(cell_to_shape.contains(cell)){
            cell_to_shape[cell].erase(p_sid);
        }
    }

    shape_to_cells.erase(p_sid);
}
const boost::unordered_set<ShapeId>& SpaceIndexes::get_shapes(const CellId& p_cid){
    if(!cell_to_shape.contains(p_cid)){
        return empty_shapes;
    }
    return cell_to_shape[p_cid];
}
const boost::unordered_set<CellId>& SpaceIndexes::get_cells(const ShapeId& p_sid){
    if(!shape_to_cells.contains(p_sid)){
        return empty_cells;
    }
    return shape_to_cells[p_sid];
}

bool SpaceIndexes::is_shape_in_same_chunk(const ShapeId& p_sid_1, const ShapeId& p_sid_2){
    const boost::unordered_set<CellId>& cells_for_1 = get_cells(p_sid_1);
    const boost::unordered_set<CellId>& cells_for_2 = get_cells(p_sid_2);
    for(auto& cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}
bool SpaceIndexes::is_shape_in_same_chunk(RawRect p_rect, const ShapeId& p_sid){
    boost::unordered_set<CellId> cells_for_1 = rect_to_cell_ids(p_rect);
    const boost::unordered_set<CellId>& cells_for_2 = get_cells(p_sid);
    for(auto& cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;   
}
bool SpaceIndexes::is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2){
    boost::unordered_set<CellId> cells_for_1 = rect_to_cell_ids(p_rect_1);
    boost::unordered_set<CellId> cells_for_2 = rect_to_cell_ids(p_rect_2);
    for(auto& cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}
bool SpaceIndexes::is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2){
    boost::unordered_set<CellId> cells_for_1 = {pos_to_cell_id(p_pos)};
    boost::unordered_set<CellId> cells_for_2 = rect_to_cell_ids(p_rect_2);
    for(auto& cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}
bool SpaceIndexes::is_shape_in_same_chunk(glm::vec2 p_pos, const ShapeId& p_sid){
    boost::unordered_set<CellId> cells_for_1 = {pos_to_cell_id(p_pos)};
    const boost::unordered_set<CellId>& cells_for_2 = get_cells(p_sid);
    for(auto& cell : cells_for_1){
        if(cells_for_2.contains(cell)){
            return true;
        }
    }
    return false;
}


bool PhysicsServer::is_shape_in_same_chunk(const ShapeId& p_sid_1, const ShapeId& p_sid_2){
    return m_space_indexes.is_shape_in_same_chunk(p_sid_1, p_sid_2);
}
bool PhysicsServer::is_shape_in_same_chunk(RawRect p_rect, const ShapeId& p_sid){
    return m_space_indexes.is_shape_in_same_chunk(p_rect, p_sid);
}
bool PhysicsServer::is_shape_in_same_chunk(RawRect p_rect_1, RawRect p_rect_2){
    return m_space_indexes.is_shape_in_same_chunk(p_rect_1, p_rect_2);
}
bool PhysicsServer::is_shape_in_same_chunk(glm::vec2 p_pos, RawRect p_rect_2){
    return m_space_indexes.is_shape_in_same_chunk(p_pos, p_rect_2);
}
bool PhysicsServer::is_shape_in_same_chunk(glm::vec2 p_pos, const ShapeId& p_sid){
    return m_space_indexes.is_shape_in_same_chunk(p_pos, p_sid);
}

ShapeId PhysicsServer::next_shape_id(){
    m_next_shape_id++;
    return m_next_shape_id;
}