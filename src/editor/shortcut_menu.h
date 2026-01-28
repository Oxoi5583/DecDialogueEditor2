#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/debug/messenger.h"
#include "ext/debug/messenger_ext.h"
#include "glm/ext/vector_float2.hpp"
#include "system/graph/camera.h"
#include "system/graph/grid.h"
#include "system/obj/graph/base.h"
#include "system/obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <cmath>
#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <vector>

class EditorShortcutMenu : public Singleton<EditorShortcutMenu>{
public:
    enum ModeFlag{
        MODE_NULL,

        MODE_GRAPH_ON_WORLD,
        MODE_GRAPH_ON_NODE,
        MODE_GRAPH_ON_NODES,

        MODE_INSPECTOR_ON_LIST,
        MODE_INSPECTOR_ON_NODE,
        MODE_INSPECTOR_ON_NODES,
    };

    struct Option{
        std::string name;
        std::vector<Option> options;
        std::function<void()> action;

        bool has_options();
    };

private:
    const char* m_root_name = "ShortcutMenu";
    std::vector<OID> m_related_obj_ids;
    vec2 m_menu_pos;
    
    Option m_option_create_node__entry = {
        "Entry",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = this->m_menu_pos;
            event.type = GraphManager::NodeType::ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__node = {
        "Node",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = this->m_menu_pos;
            event.type = GraphManager::NodeType::NODE;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__option = {
        "Option",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = this->m_menu_pos;
            event.type = GraphManager::NodeType::OPTION;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__repeater = {
        "Repeater",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = this->m_menu_pos;
            event.type = GraphManager::NodeType::REPEATER;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__module_entry = {
        "Module Entry",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = this->m_menu_pos;
            event.type = GraphManager::NodeType::MODULE_ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__module_node = {
        "Module Node",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = this->m_menu_pos;
            event.type = GraphManager::NodeType::MODULE_NODE;
            EventServer::Ref()->emit(event);
        }
    };

    Option m_option_create_node = {
        "Create Node",
        {
            m_option_create_node__entry,
            m_option_create_node__node,
            m_option_create_node__option,
            m_option_create_node__repeater,
            m_option_create_node__module_entry,
            m_option_create_node__module_node,
        },
        std::function<void()>()
    };
    
    Option m_option_connect = {
        "Connect...",
        {},
        [this](){
            if(this->m_related_obj_ids.empty()){
                return;
            }

            OID id = this->m_related_obj_ids[0];
            if(!ObjectServer::Ref()->is_id_valid(id)){
                return;
            }

            EventStartConnect event;
            event.id = id;
            EventServer::Ref()->emit(event);
        }
    };
    
    Option m_option_create_node_at_cam__entry = {
        "Entry",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__node = {
        "Node",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::NODE;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__option = {
        "Option",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::OPTION;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__repeater = {
        "Repeater",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::REPEATER;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__module_entry = {
        "Module Entry",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::MODULE_ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__module_node = {
        "Module Node",
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::MODULE_NODE;
            EventServer::Ref()->emit(event);
        }
    };

    Option m_option_create_node_at_cam = {
        "Create Node",
        {
            m_option_create_node_at_cam__entry,
            m_option_create_node_at_cam__node,
            m_option_create_node_at_cam__option,
            m_option_create_node_at_cam__repeater,
            m_option_create_node_at_cam__module_entry,
            m_option_create_node_at_cam__module_node,
        },
        std::function<void()>()
    };

    Option m_option_edit_node = {
        "Edit",
        {},
        [this](){
            auto& ids = this->m_related_obj_ids;
            for(OID& id : ids){
                EventOpenDetailsWindow event;
                event.id = id;
                EventServer::Ref()->emit(event);
            }
        }
    };
    Option m_option_delete_node = {
        "Delete node",
        {},
        [this](){
            auto& ids = this->m_related_obj_ids;
            for(OID& id : ids){
                if(ObjectServer::Ref()->is_id_valid(id)){
                    ObjectServer::Ref()->get_instance<ObjectBase>(id)->queue_free();
                }
            }
        }
    };
    Option m_option_delete_nodes = {
        "Delete nodes",
        {},
        [this](){
            auto& ids = this->m_related_obj_ids;
            for(OID& id : ids){
                if(ObjectServer::Ref()->is_id_valid(id)){
                    ObjectServer::Ref()->get_instance<ObjectBase>(id)->queue_free();
                }
            }
        }
    };

    void m_sorting_ids_for_align(std::vector<OID>& p_ids);

    Option m_option_align_nodes__to_rectangle = {
        "To rectangle",
        {},
        [this](){
            auto& ids = this->m_related_obj_ids;
            int count = ids.size();
            int width = std::ceil(std::sqrt((float)count));
            int height = std::ceil((float)count / (float)width);

            m_sorting_ids_for_align(ids);
            OID& first_id = ids[0];
            GraphBase* first_obj = ObjectServer::Ref()->get_instance<GraphBase>(first_id);
            vec2 first_pos = first_obj->get_position();

            double interval = GraphGrid::Ref()->grid_interval;
            vec2 new_first_pos = {
                std::floor(first_pos.x / interval) * interval,
                std::floor(first_pos.y / interval) * interval
            };

            int index = 0;
            for(size_t x = 0; x < width; x++){
                for(size_t y = 0; y < height; y++){
                    if(index >= ids.size()){
                        break;
                    }

                    OID id = ids[index];
                    if(!ObjectServer::Ref()->is_id_valid(id)){
                        continue;
                    }

                    vec2 new_pos = new_first_pos + vec2(
                        3 * interval * x,
                        3 * interval * y
                    );

                    GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
                    obj->set_position(new_pos);

                    index++;
                }
            }

            for(OID& id : ids){
                if(!ObjectServer::Ref()->is_id_valid(id)){
                    continue;
                }
                ObjectServer::Ref()->get_instance<GraphBase>(id)->select();
            }
        }
    };
    Option m_option_align_nodes__to_vertical = {
        "To vertical",
        {},
        [this](){
            auto& ids = this->m_related_obj_ids;
            int count = ids.size();

            m_sorting_ids_for_align(ids);
            OID& first_id = ids[0];
            GraphBase* first_obj = ObjectServer::Ref()->get_instance<GraphBase>(first_id);
            vec2 first_pos = first_obj->get_position();

            double interval = GraphGrid::Ref()->grid_interval;
            vec2 new_first_pos = {
                std::floor(first_pos.x / interval) * interval,
                std::floor(first_pos.y / interval) * interval
            };

            GraphManager::NodeType current_type = first_obj->get_type();
            int x = 0;
            int y = 0;
            for(OID& id : ids){
                if(!ObjectServer::Ref()->is_id_valid(id)){
                    continue;
                }

                GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
                if(current_type != obj->get_type()){
                    x++;
                    y = 0;
                    current_type = obj->get_type();
                }

                vec2 new_pos = new_first_pos + vec2(
                    3 * interval * x,
                    3 * interval * y
                );

                obj->set_position(new_pos);

                y++;
            }

            for(OID& id : ids){
                if(!ObjectServer::Ref()->is_id_valid(id)){
                    continue;
                }
                ObjectServer::Ref()->get_instance<GraphBase>(id)->select();
            }
        }
    };
    Option m_option_align_nodes__to_horizontal = {
        "To horizontal",
        {},
        [this](){
            auto& ids = this->m_related_obj_ids;
            int count = ids.size();

            m_sorting_ids_for_align(ids);
            OID& first_id = ids[0];
            GraphBase* first_obj = ObjectServer::Ref()->get_instance<GraphBase>(first_id);
            vec2 first_pos = first_obj->get_position();

            double interval = GraphGrid::Ref()->grid_interval;
            vec2 new_first_pos = {
                std::floor(first_pos.x / interval) * interval,
                std::floor(first_pos.y / interval) * interval
            };

            GraphManager::NodeType current_type = first_obj->get_type();
            int x = 0;
            int y = 0;
            for(OID& id : ids){
                if(!ObjectServer::Ref()->is_id_valid(id)){
                    continue;
                }

                GraphBase* obj = ObjectServer::Ref()->get_instance<GraphBase>(id);
                if(current_type != obj->get_type()){
                    x = 0;
                    y++;
                    current_type = obj->get_type();
                }

                vec2 new_pos = new_first_pos + vec2(
                    3 * interval * x,
                    3 * interval * y
                );

                obj->set_position(new_pos);

                x++;
            }

            for(OID& id : ids){
                if(!ObjectServer::Ref()->is_id_valid(id)){
                    continue;
                }
                ObjectServer::Ref()->get_instance<GraphBase>(id)->select();
            }

        }
    };
    Option m_option_align_nodes = {
        "Align nodes",
        {
            m_option_align_nodes__to_rectangle,
            m_option_align_nodes__to_vertical,
            m_option_align_nodes__to_horizontal,
        },
        [this](){}
    };


    Option m_null = {
        "Root",
        {},
        std::function<void()>()
    };
    Option m_graph_on_world = {
        "Root",
        {
            m_option_create_node,
        },
        std::function<void()>()
    };
    Option m_graph_on_node = {
        "Root",
        {
            m_option_edit_node,
            m_option_connect,
            m_option_create_node,
            m_option_delete_node,
        },
        std::function<void()>()
    };
    Option m_graph_on_nodes = {
        "Root",
        {
            m_option_edit_node,
            m_option_create_node,
            m_option_delete_nodes,
            m_option_align_nodes,
        },
        std::function<void()>()
    };
    
    Option m_inspector_on_list = {
        "Root",
        {
            m_option_create_node_at_cam,
        },
        std::function<void()>()
    };
    Option m_inspector_on_node = {
        "Root",
        {
            m_option_edit_node,
            m_option_create_node_at_cam,
            m_option_delete_node,
        },
        std::function<void()>()
    };
    Option m_inspector_on_nodes = {
        "Root",
        {
            m_option_edit_node,
            m_option_create_node_at_cam,
            m_option_delete_nodes,
            m_option_align_nodes,
        },
        std::function<void()>()
    };

    std::map<ModeFlag, Option> m_menu = {
        {ModeFlag::MODE_NULL, m_null},

        /*   Mode Graph   */
        {ModeFlag::MODE_GRAPH_ON_WORLD, m_graph_on_world},
        {ModeFlag::MODE_GRAPH_ON_NODE , m_graph_on_node},
        {ModeFlag::MODE_GRAPH_ON_NODES , m_graph_on_nodes},

        /*   Mode Inspector   */
        {ModeFlag::MODE_INSPECTOR_ON_LIST , m_inspector_on_list},
        {ModeFlag::MODE_INSPECTOR_ON_NODE , m_inspector_on_node},
        {ModeFlag::MODE_INSPECTOR_ON_NODES , m_inspector_on_nodes},
    };

    ModeFlag m_current_mode = ModeFlag::MODE_GRAPH_ON_NODE;


    void m_control_mode();
    void m_draw_menu();
    void m_draw_options(std::vector<Option>& p_options);
public:
    EditorShortcutMenu() = default;
    ~EditorShortcutMenu() = default;

    void update();
    
    bool is_opened();
};

/*
Note :

2025/11/19
This class is working in dirty logic.
there should be a list of condiftion flags
Adding class of option and detecting its flags exists would be beteer.
Also should use event to free node instead.
Please be noticed if writing simular things in future.

*/