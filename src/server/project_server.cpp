#include "server/project_server.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/file_server.h"
#include "server/object_server.h"
#include "system/obj/graph/base.h"
#include "system/obj/graph/connection_line.h"
#include "system/obj/graph/manager.h"
#include <cstddef>
#include <nlohmann/json.hpp>

ProjectServer::ProjectServer(){
    FileServer::Ref()->get_root()["projects"].add_modified_callback([this](){
        this->m_refresh_projects_data();
    });
    FString init_file = this->create_file();
    m_current_workspace = init_file;
}
ProjectServer::~ProjectServer(){
    m_clear_files_if_not_saved();
}

void ProjectServer::process(){
    m_refresh_projects_data();
    m_free_graph_nodes_if_workspace_not_exists();
}

FPathWrapper& ProjectServer::get_project_root(){
    FPathWrapper& path = FileServer::Ref()->get_root()["projects"];
    if(path.contains(m_current_project)){
        return path[m_current_project.c_str()];
    }else{
        m_current_project = default_name;
        return path[m_current_project.c_str()];
    }
}
void ProjectServer::set_project(std::string p_name){
    FPathWrapper& p = this->get_project_root();
    if(!p.contains(p_name)){
        p.create_dir(p_name);
        INFO_MSG("Project folder created : " << p_name);
    }

    m_current_project = p_name;
}
void ProjectServer::m_freeze_graph_nodes_if_in_workspace(){
    std::vector<OID> ids = ObjectServer::Ref()->get_all_ids();
    for(OID& id : ids){
        GraphBase* ptr1 = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(ptr1){
            if(ptr1->get_workspace_id() == ProjectServer::Ref()->get_workspace_uid()){
                ptr1->set_freeze(false);
            }else{
                ptr1->set_freeze(true);
            }
        }
        GraphConnectionLine* ptr2 = ObjectServer::Ref()->get_instance<GraphConnectionLine>(id);
        if(ptr2){
            OID from_id = ptr2->get_from_id();
            OID to_id = ptr2->get_to_id();
            
            GraphBase* from_ptr = ObjectServer::Ref()->get_instance<GraphBase>(from_id);
            if(from_ptr){
                if(from_ptr->get_workspace_id() == ProjectServer::Ref()->get_workspace_uid()){
                    ptr2->set_freeze(false);
                }else{
                    ptr2->set_freeze(true);
                }
            }
            GraphBase* to_ptr = ObjectServer::Ref()->get_instance<GraphBase>(to_id);
            if(to_ptr){
                if(to_ptr->get_workspace_id() == ProjectServer::Ref()->get_workspace_uid()){
                    ptr2->set_freeze(false);
                }else{
                    ptr2->set_freeze(true);
                }
            }
        }
    }
}
void ProjectServer::m_free_graph_nodes_if_workspace_not_exists(){
    std::vector<OID> ids = ObjectServer::Ref()->get_all_ids();
    for(OID& id : ids){
        GraphBase* ptr1 = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(ptr1){
            std::string workspace_uid = ptr1->get_workspace_id();
            if(!get_project_data().contains(workspace_uid)){
                ptr1->queue_free();
            }
        }
        GraphConnectionLine* ptr2 = ObjectServer::Ref()->get_instance<GraphConnectionLine>(id);
        if(ptr2){
            OID from_id = ptr2->get_from_id();
            OID to_id = ptr2->get_to_id();
            
            GraphBase* from_ptr = ObjectServer::Ref()->get_instance<GraphBase>(from_id);
            if(from_ptr){
                std::string workspace_uid = from_ptr->get_workspace_id();
                if(!get_project_data().contains(workspace_uid)){
                    ptr2->queue_free();
                }
            }
            GraphBase* to_ptr = ObjectServer::Ref()->get_instance<GraphBase>(to_id);
            if(to_ptr){
                std::string workspace_uid = to_ptr->get_workspace_id();
                if(!get_project_data().contains(workspace_uid)){
                    ptr2->queue_free();
                }
            }
        }
    }
}
void ProjectServer::set_workspace(std::string p_uid){
    auto proj_data = this->get_project_data();
    if(!proj_data.contains(p_uid)){
        return;
    }

    m_current_workspace = p_uid;

    m_freeze_graph_nodes_if_in_workspace();
}
void ProjectServer::m_clear_files_if_not_saved(){
    if(m_current_project == default_name){
        this->get_project_root().clear();
    }
}
void ProjectServer::m_refresh_projects_data(){
    std::map<std::string, std::map<std::string, ProjectWorkSpace>>().swap(m_projects);

    FPathWrapper& root = FileServer::Ref()->get_root()["projects"];

    for(auto& p : root.children){
        m_projects.emplace(p.second.get_name(), std::map<std::string, ProjectWorkSpace>());
        auto& proj = m_projects[p.second.get_name().c_str()];

        size_t i = 0;
        for(auto& f : p.second.children){
            FPathWrapper& file = f.second;
            if(file.is_json()){
                nlohmann::json data = file.get_json();
                if(is_project_file_valid(data)){
                    ProjectWorkSpace pd;
                    pd.load_pri = file.last_write_time;
                    pd.name = data["name"];
                    pd.path = file.path.string();
                    pd.path_wrapper = &file;
                    pd.uid = f.first;
                    pd.data = data;
                    pd.size = {file.get_size()};
                    pd.is_selected = (pd.uid == m_current_workspace);

                    proj.emplace(pd.uid, pd);
                    i++;
                }
            }
        }
    }
}

FString ProjectServer::create_file(){
    auto& proj = get_project_root();

    FString new_file_name = RandomCode(25).get();
    proj.create_file(new_file_name);

    nlohmann::json j;
    j["name"] = "New Project";
    proj[new_file_name].append_text(j.dump(4));
    
    return new_file_name;
}

bool ProjectServer::is_project_file_valid(nlohmann::json& p_data){
    for(auto& cl : m_json_checklist){
        if(!p_data.contains(cl.field)){
            return false;
        }
        switch (cl.type) {
            case STRING:{
                if(!p_data[cl.field].is_string()) return false;
                break;
            }
            case NUMERIC:{
                if(!p_data[cl.field].is_number()) return false;
                break;
            }
            case BOOLEAN:{
                if(!p_data[cl.field].is_boolean()) return false;
                break;
            }
            case ARRAY:{
                if(!p_data[cl.field].is_array()) return false;
                break;
            }
            case DICTIONARY:{
                if(!p_data[cl.field].is_object()) return false;
                break;
            }
        }
    }
    return true;
}
std::map<std::string, ProjectWorkSpace> ProjectServer::get_project_data(){
    if(m_projects.contains(m_current_project)){
        return m_projects[m_current_project];
    }
    m_current_project = default_name;
    return m_projects[m_current_project];
}
ProjectWorkSpace ProjectServer::get_workspace_data(){
    return get_project_data()[m_current_workspace];
}

void ProjectServer::save_workspace(){
    auto workspace = get_workspace_data();
    workspace.path_wrapper->truncate_text();
    workspace.path_wrapper->append_text(workspace.data.dump(4));
}

std::string ProjectServer::get_workspace_uid(){
    return m_current_workspace;
}