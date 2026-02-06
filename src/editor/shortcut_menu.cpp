#include "editor/shortcut_menu.h"
#include "DecToolsBox/debug/messenger.h"
#include "imgui/imgui.h"
#include "system/obj/graph/manager.h"
#include "server/event_server.h"
#include "server/events.h"
#include "server/mouse_server.h"
#include "server/object_base.h"
#include "server/object_server.h"
#include <vector>


bool EditorShortcutMenu::Option::has_options(){
    return !options.empty();
}

void EditorShortcutMenu::m_control_mode(){
    std::vector<OID>().swap(datapipeline.ids);
    std::vector<std::string>().swap(datapipeline.strs);
    std::vector<vec2>().swap(datapipeline.vectors);

    if(EventServer::Ref()->has<EventLeftPanelSelectedItemHovered>()){
        EventLeftPanelSelectedItemHovered event = EventServer::Ref()->poll_first<EventLeftPanelSelectedItemHovered>();
        auto ids = event.ids;

        int done = 0;

        for(OID& id : ids){
            if(ObjectServer::Ref()->is_id_valid(id)){
                datapipeline.ids.push_back(id);
                done++;
            }
        }

        if(done == 1){
            m_current_mode = ModeFlag::MODE_INSPECTOR_ON_NODE;
        }
        
        if(done > 1){
            m_current_mode = ModeFlag::MODE_INSPECTOR_ON_NODES;
        }

        if(done > 0){
            return;
        }
    }

    if(EventServer::Ref()->has<EventLeftPanelItemHovered>()){
        EventLeftPanelItemHovered event = EventServer::Ref()->poll_first<EventLeftPanelItemHovered>();
        if(ObjectServer::Ref()->is_id_valid(event.id)){
            datapipeline.ids.push_back(event.id);

            m_current_mode = ModeFlag::MODE_INSPECTOR_ON_NODE;
            return;
        }
    }

    if(EventServer::Ref()->has<EventLeftPanelHovered>()){
        m_current_mode = ModeFlag::MODE_INSPECTOR_ON_LIST;
        return;
    }

    if(MouseServer::Ref()->is_mouse_in_viewport()){
        if(EventServer::Ref()->has<EventSelectedObjHovering>()){
            auto list = EventServer::Ref()->poll<EventMouseSelectedObj>();
            if(list.size() > 1){
                for(auto& e : list){
                    datapipeline.ids.push_back(e.obj_id);
                }
                datapipeline.vectors.push_back(MouseServer::Ref()->get_mouse_world_position());
                m_current_mode = ModeFlag::MODE_GRAPH_ON_NODES;
                return;
            }
        }
    }

    if(MouseServer::Ref()->is_mouse_in_viewport()){
        if(EventServer::Ref()->has<EventMouseHoverOnWorld>()){
            m_current_mode = ModeFlag::MODE_GRAPH_ON_WORLD;
            datapipeline.vectors.push_back(MouseServer::Ref()->get_mouse_world_position());
            return;
        }
    }

    if(MouseServer::Ref()->is_mouse_in_viewport()){
        if(EventServer::Ref()->has<EventMouseHoverObjLastFrame>()){
            EventMouseHoverObjLastFrame event = EventServer::Ref()->poll_first<EventMouseHoverObjLastFrame>();
            OID id = event.obj_id;
            if(ObjectServer::Ref()->is_id_valid(id)){
                datapipeline.ids.push_back(event.obj_id);

                m_current_mode = ModeFlag::MODE_GRAPH_ON_NODE;
                datapipeline.vectors.push_back(MouseServer::Ref()->get_mouse_world_position());
                return;
            }
        }
    }

    if(EventServer::Ref()->has<EventHoveredExplorerList>()){
        if(ProjectServer::Ref()->get_workspace_count() > 1){
            EventHoveredExplorerList evt = EventServer::Ref()->poll_first<EventHoveredExplorerList>();
            this->datapipeline.strs.push_back(evt.workspace_uid);
            m_current_mode = ModeFlag::MODE_EXPLORER_ON_LIST_MORE_THAN_WS;
            return;
        }else{
            EventHoveredExplorerList evt = EventServer::Ref()->poll_first<EventHoveredExplorerList>();
            this->datapipeline.strs.push_back(evt.workspace_uid);
            m_current_mode = ModeFlag::MODE_EXPLORER_ON_LIST_ONLY_ONE_WS;
            return;
        }
    }

    m_current_mode = ModeFlag::MODE_NULL;
}

void EditorShortcutMenu::m_draw_menu(){
    Option& root = m_menu[m_current_mode];
    if (ImGui::IsMouseClicked(1)) {
        m_control_mode();

        if(m_current_mode == ModeFlag::MODE_NULL) return;
    
        ImGui::OpenPopup(m_root_name);
    }
    if (ImGui::BeginPopup(m_root_name, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        if(ImGui::IsWindowHovered()){
            EventLockedAll evt1;
            EventServer::Ref()->emit(evt1);
            EventMouseHoverObj evt2;
            EventServer::Ref()->emit(evt2);
        }
        m_draw_options(root.options);
        ImGui::EndPopup();
    }
}

void EditorShortcutMenu::m_draw_options(std::vector<Option>& p_options){
    for(auto &opt : p_options){
        if(opt.has_options()){
            if(ImGui::BeginMenu(opt.name->c_str())){
                if(ImGui::IsWindowHovered()){
                    EventLockedAll evt1;
                    EventServer::Ref()->emit(evt1);
                    EventMouseHoverObj evt2;
                    EventServer::Ref()->emit(evt2);
                }
                m_draw_options(opt.options);
                ImGui::EndMenu();
            }
        }else{
            if(ImGui::MenuItem(opt.name->c_str())){
                opt.action();
            }
        }
    }
}

void EditorShortcutMenu::update(){
    m_draw_menu();
}

bool EditorShortcutMenu::is_opened(){
    return ImGui::IsPopupOpen(m_root_name);
}

void EditorShortcutMenu::m_sorting_ids_for_align(std::vector<OID>& p_ids){
    std::sort(p_ids.begin(), p_ids.end(), 
            [](OID& x, OID& y){
            GraphBase* x_ptr = ObjectServer::Ref()->get_instance<GraphBase>(x);
            GraphBase* y_ptr = ObjectServer::Ref()->get_instance<GraphBase>(y);
            return (int)x_ptr->get_type() < (int)y_ptr->get_type();
        }
    );
}