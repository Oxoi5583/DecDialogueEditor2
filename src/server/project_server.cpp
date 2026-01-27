#include "server/project_server.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/file_server.h"
#include <nlohmann/json.hpp>

FPathWrapper& ProjectServer::get_project(){
    FPathWrapper& path = FileServer::Ref()->get_root()["projects"];
    if(path.contains(m_current_project)){
        return path[m_current_project.c_str()];
    }else{
        m_current_project = default_name;
        return path[m_current_project.c_str()];
    }
}
ProjectServer::ProjectServer(){
}
ProjectServer::~ProjectServer(){
    m_clear_files_if_not_saved();
}
void ProjectServer::set_project(std::string p_name){
    FPathWrapper& p = this->get_project();
    if(!p.contains(p_name)){
        p.create_dir(p_name);
        INFO_MSG("Project folder created : " << p_name);
    }

    m_current_project = p_name;
}
void ProjectServer::m_clear_files_if_not_saved(){
    if(m_current_project == default_name){
        this->get_project().clear();
    }
}

void ProjectServer::create_file(){
    auto& proj = get_project();

    FString new_file_name = RandomCode(25).get();
    proj.create_file(new_file_name);

    nlohmann::json j;
    j["name"] = "New Project";
    proj[new_file_name].append_text(j.dump(4));
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