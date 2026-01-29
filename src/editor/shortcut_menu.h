#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/debug/messenger.h"
#include "editor/components/popup_window.h"
#include "ext/debug/messenger_ext.h"
#include "glm/ext/vector_float2.hpp"
#include "server/file_server.h"
#include "server/project_server.h"
#include "server/ui_text_bank.h"
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
#include <utility>
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

        MODE_EXPLORER_ON_LIST_ONLY_ONE_WS,
        MODE_EXPLORER_ON_LIST_MORE_THAN_WS,
    };

    struct Option{
        UiText* name;
        std::vector<Option> options;
        std::function<void()> action;

        bool has_options();
    };

private:
    const char* m_root_name = "ShortcutMenu";

    struct{
        std::vector<OID> ids;
        std::vector<std::string> strs;
        std::vector<vec2> vectors;
    } datapipeline;
        
    Option m_option_create_node__entry = {
        &UiTextBank::Ref()->OptionEntry,
        {},
        [this](){
            if(datapipeline.vectors.empty()){
                return;
            }

            EventSpawnNode event;
            event.spawn_pos = this->datapipeline.vectors[0];
            event.type = GraphManager::NodeType::ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__node = {
        &UiTextBank::Ref()->OptionNode,
        {},
        [this](){
            if(datapipeline.vectors.empty()){
                return;
            }

            EventSpawnNode event;
            event.spawn_pos = this->datapipeline.vectors[0];
            event.type = GraphManager::NodeType::NODE;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__option = {
        &UiTextBank::Ref()->OptionOption,
        {},
        [this](){
            if(datapipeline.vectors.empty()){
                return;
            }

            EventSpawnNode event;
            event.spawn_pos = this->datapipeline.vectors[0];
            event.type = GraphManager::NodeType::OPTION;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__repeater = {
        &UiTextBank::Ref()->OptionRepeater,
        {},
        [this](){
            if(datapipeline.vectors.empty()){
                return;
            }

            EventSpawnNode event;
            event.spawn_pos = this->datapipeline.vectors[0];
            event.type = GraphManager::NodeType::REPEATER;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__module_entry = {
        &UiTextBank::Ref()->OptionModuleEntry,
        {},
        [this](){
            if(datapipeline.vectors.empty()){
                return;
            }

            EventSpawnNode event;
            event.spawn_pos = this->datapipeline.vectors[0];
            event.type = GraphManager::NodeType::MODULE_ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node__module_node = {
        &UiTextBank::Ref()->OptionModuleNode,
        {},
        [this](){
            if(datapipeline.vectors.empty()){
                return;
            }

            EventSpawnNode event;
            event.spawn_pos = this->datapipeline.vectors[0];
            event.type = GraphManager::NodeType::MODULE_NODE;
            EventServer::Ref()->emit(event);
        }
    };

    Option m_option_create_node = {
        &UiTextBank::Ref()->OptionCreateNode,
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
        &UiTextBank::Ref()->OptionConnect,
        {},
        [this](){
            if(this->datapipeline.ids.empty()){
                return;
            }

            OID id = this->datapipeline.ids[0];
            if(!ObjectServer::Ref()->is_id_valid(id)){
                return;
            }

            EventStartConnect event;
            event.id = id;
            EventServer::Ref()->emit(event);
        }
    };

    Option m_option_create_node_at_cam__entry = {
        &UiTextBank::Ref()->OptionEntry,
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__node = {
        &UiTextBank::Ref()->OptionNode,
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::NODE;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__option = {
        &UiTextBank::Ref()->OptionOption,
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::OPTION;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__repeater = {
        &UiTextBank::Ref()->OptionRepeater,
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::REPEATER;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__module_entry = {
        &UiTextBank::Ref()->OptionModuleEntry,
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::MODULE_ENTRY;
            EventServer::Ref()->emit(event);
        }
    };
    Option m_option_create_node_at_cam__module_node = {
        &UiTextBank::Ref()->OptionModuleNode,
        {},
        [this](){
            EventSpawnNode event;
            event.spawn_pos = GraphCamera::Ref()->get_target();
            event.type = GraphManager::NodeType::MODULE_NODE;
            EventServer::Ref()->emit(event);
        }
    };

    Option m_option_create_node_at_cam = {
        &UiTextBank::Ref()->OptionCreateNode,
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
        &UiTextBank::Ref()->OptionEdit,
        {},
        [this](){
            auto& ids = this->datapipeline.ids;
            for(OID& id : ids){
                EventOpenDetailsWindow event;
                event.id = id;
                EventServer::Ref()->emit(event);
            }
        }
    };
    Option m_option_delete_node = {
        &UiTextBank::Ref()->OptionDeleteNode,
        {},
        [this](){
            auto& ids = this->datapipeline.ids;
            for(OID& id : ids){
                if(ObjectServer::Ref()->is_id_valid(id)){
                    ObjectServer::Ref()->get_instance<ObjectBase>(id)->queue_free();
                }
            }
        }
    };
    Option m_option_delete_nodes = {
        &UiTextBank::Ref()->OptionDeleteNodes,
        {},
        [this](){
            auto& ids = this->datapipeline.ids;
            for(OID& id : ids){
                if(ObjectServer::Ref()->is_id_valid(id)){
                    ObjectServer::Ref()->get_instance<ObjectBase>(id)->queue_free();
                }
            }
        }
    };

    void m_sorting_ids_for_align(std::vector<OID>& p_ids);

    Option m_option_align_nodes__to_rectangle = {
        &UiTextBank::Ref()->OptionAlignToRectangle,
        {},
        [this](){
            auto& ids = this->datapipeline.ids;
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
        &UiTextBank::Ref()->OptionAlignToVertical,
        {},
        [this](){
            auto& ids = this->datapipeline.ids;
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
        &UiTextBank::Ref()->OptionAlignToHorizontal,
        {},
        [this](){
            auto& ids = this->datapipeline.ids;
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
        &UiTextBank::Ref()->OptionAlignNodes,
        {
            m_option_align_nodes__to_rectangle,
            m_option_align_nodes__to_vertical,
            m_option_align_nodes__to_horizontal,
        },
        [this](){}
    };

    Option m_null = {
        &UiTextBank::Ref()->OptionRoot,
        {},
        std::function<void()>()
    };
    Option m_graph_on_world = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_option_create_node,
        },
        std::function<void()>()
    };
    Option m_graph_on_node = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_option_edit_node,
            m_option_connect,
            m_option_create_node,
            m_option_delete_node,
        },
        std::function<void()>()
    };
    Option m_graph_on_nodes = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_option_edit_node,
            m_option_create_node,
            m_option_delete_nodes,
            m_option_align_nodes,
        },
        std::function<void()>()
    };

    Option m_inspector_on_list = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_option_create_node_at_cam,
        },
        std::function<void()>()
    };
    Option m_inspector_on_node = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_option_edit_node,
            m_option_create_node_at_cam,
            m_option_delete_node,
        },
        std::function<void()>()
    };
    Option m_inspector_on_nodes = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_option_edit_node,
            m_option_create_node_at_cam,
            m_option_delete_nodes,
            m_option_align_nodes,
        },
        std::function<void()>()
    };

    Option m_explorer_delete_workspace = {
        &UiTextBank::Ref()->OptionDeleteWorkspace,
        {},
        [this](){
            if(datapipeline.strs.empty()){
                return;
            }
            if(ProjectServer::Ref()->get_project_data().size() == 1){
                return;
            }

            std::string hovered_uid = this->datapipeline.strs[0];
            std::string current_uid = ProjectServer::Ref()->get_workspace_uid();

            std::string change_to_uid = current_uid;

            if(hovered_uid == current_uid){
                auto workspaces = ProjectServer::Ref()->get_project_data_sorted();
                std::string new_ws_uid;
                for(auto& ws : workspaces){
                    if(ws.uid != current_uid){
                        new_ws_uid = ws.uid;
                        break;
                    }
                }
                change_to_uid = new_ws_uid;
            }

            typedef std::string WindowUID;
            typedef PopupWindow* WindowPtr;
            static std::pair<WindowUID ,WindowPtr> window_wrapper;

            if(PopupWindowManager::Ref()->is_window_exists(window_wrapper.first)){
            window_wrapper.second->close();
            }

            PopupWindow* window = ObjectServer::Ref()->queue_create<PopupWindow>();
            WindowUID window_uid = window->get_uid();
            WindowPtr window_ptr = window;

            window_wrapper.first = window_uid;
            window_wrapper.second = window_ptr;

            window->set_content(UiTextBank::Ref()->ConfirmDelete);
            window->add_option(UiTextBank::Ref()->Yes, [window, change_to_uid, hovered_uid](){
                ProjectServer::Ref()->set_workspace(change_to_uid);
                ProjectServer::Ref()->get_project_root().remove(hovered_uid);
                window->close();
            });
            window->add_option(UiTextBank::Ref()->No, [window, change_to_uid, hovered_uid](){
                window->close();
            });

        }
    };
    Option m_explorer_rename_workspace = {
        &UiTextBank::Ref()->OptionRenameWorkspace,
        {},
        [this](){
            if(datapipeline.strs.empty()){
                return;
            }

            typedef std::string WindowUID;
            typedef PopupWindow* WindowPtr;
            static std::pair<WindowUID ,WindowPtr> window_wrapper;

            if(PopupWindowManager::Ref()->is_window_exists(window_wrapper.first)){
            window_wrapper.second->close();
            }

            PopupWindow* window = ObjectServer::Ref()->queue_create<PopupWindow>();
            WindowUID hovered_uid = this->datapipeline.strs[0];
            WindowUID window_uid = window->get_uid();
            WindowPtr window_ptr = window;

            window_wrapper.first = window_uid;
            window_wrapper.second = window_ptr;

            window->set_content(UiTextBank::Ref()->RenameWorkspace);
            window->add_input("Value :", PopupWindow::InputType::STRING);
            window->add_option(UiTextBank::Ref()->Confirm, [window, hovered_uid](){
                std::string value = window->get_input_string(0);
                if(value.size() > 0){
                    ProjectServer::Ref()->edit_workspace(hovered_uid, "name", value);
                    window->close();
                }
            });
        }
    };

    Option m_explorer_on_list_only_one_workspace = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_explorer_rename_workspace,
        },
        std::function<void()>()
    };

    Option m_explorer_on_list_more_than_one_workspace = {
        &UiTextBank::Ref()->OptionRoot,
        {
            m_explorer_rename_workspace,
            m_explorer_delete_workspace,
        },
        std::function<void()>()
    };



    std::map<ModeFlag, Option> m_menu = {
        {ModeFlag::MODE_NULL , m_null},

        /*   Mode Graph   */
        {ModeFlag::MODE_GRAPH_ON_WORLD      , m_graph_on_world},
        {ModeFlag::MODE_GRAPH_ON_NODE       , m_graph_on_node},
        {ModeFlag::MODE_GRAPH_ON_NODES      , m_graph_on_nodes},

        /*   Mode Inspector   */
        {ModeFlag::MODE_INSPECTOR_ON_LIST   , m_inspector_on_list},
        {ModeFlag::MODE_INSPECTOR_ON_NODE   , m_inspector_on_node},
        {ModeFlag::MODE_INSPECTOR_ON_NODES  , m_inspector_on_nodes},

        /*   Mode Explorer   */
        {ModeFlag::MODE_EXPLORER_ON_LIST_ONLY_ONE_WS    , m_explorer_on_list_only_one_workspace},
        {ModeFlag::MODE_EXPLORER_ON_LIST_MORE_THAN_WS    , m_explorer_on_list_more_than_one_workspace},
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