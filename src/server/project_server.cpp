#include "server/project_server.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "server/file_server.h"
#include "server/object_server.h"
#include "system/obj/fstream/base.h"
#include "system/obj/fstream/file.h"
#include "system/obj/fstream/folder.h"
#include "system/obj/graph/base.h"
#include "system/obj/graph/connection_line.h"
#include "system/obj/graph/manager.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>


bool ProjectPayload::is_project_exists(){
    return ProjectServer::Ref()->m_projects.contains(this->project);
}
bool ProjectPayload::is_workspace_exists(){
    if(!is_project_exists()){
        return false;
    }
    return ProjectServer::Ref()->m_projects[this->project].spaces.contains(this->workspace);
}
bool ProjectPayload::is_data_exists(){
    if(!(is_project_exists() && is_workspace_exists())){
        return false;
    }
    nlohmann::json& root = ProjectServer::Ref()->m_projects[this->project].spaces[this->workspace].data;
    for(PString key : this->keys){
        if(root.contains(key)){
            root = root[key];
        }else{
            return false;
        }
    }
    return true;
}

void Project::save_as(PString p_path){
    for(auto& it : spaces){
        it.second.save();
    }
    FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(this->folder_id);
    if(folder){
    }
}

void Workspace::save(){
    is_saved = true;
    this->data["workspace_info"]["updated_at"] = std::chrono::utc_clock::now().time_since_epoch().count();
    FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(this->file_id);
    if(file){
        file->truncate();
        file->append(this->data.dump(4));
    }
}

void ProjectServer::process(){
    m_freeze_non_workspace_obj();
    m_update_workspace_selection();
    m_refresh_display_data();
}
void ProjectServer::init(){
    Project proj;
    proj.code = "temp";

    FStreamLink link;
    link.push_back("projects");
    link.push_back("temp");
    proj.folder_id = FileServer::Ref()->get_fstream_obj(link);
    m_projects.emplace("temp", proj);

    PString ws_code = this->create_workspace();
    this->go_to_workspace(ws_code);
}

ProjectServer::~ProjectServer(){
    FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(m_projects["temp"].folder_id);
    folder->clear();
}

void ProjectServer::m_update_workspace_selection(){
    for(auto& it : m_projects[m_project_uid].spaces){
        Workspace& space = it.second;
        if(space.code == m_workspace_uid){
            space.is_selected = true;
        }else{
            space.is_selected = false;
        }
    }
}
void ProjectServer::m_freeze_non_workspace_obj(){
    for(OID id :ObjectServer::Ref()->get_all_ids()){
        GraphBase* base = ObjectServer::Ref()->get_instance<GraphBase>(id);
        if(base){
            if(base->get_workspace_id() == m_workspace_uid){
                base->set_freeze(false);
            }else{
                base->set_freeze(true);
            }
            continue;
        }
        
        GraphConnectionLine* line = ObjectServer::Ref()->get_instance<GraphConnectionLine>(id);
        if(line){
            OID fm_id = line->get_from_id();
            OID to_id = line->get_to_id();
            GraphBase* fm_base = ObjectServer::Ref()->get_instance<GraphBase>(fm_id);
            if(fm_base){
                if(fm_base->get_workspace_id() == m_workspace_uid){
                    line->set_freeze(false);
                }else{
                    line->set_freeze(true);
                }
                continue;
            }

            GraphBase* to_base = ObjectServer::Ref()->get_instance<GraphBase>(to_id);
            if(to_base){
                if(to_base->get_workspace_id() == m_workspace_uid){
                    line->set_freeze(false);
                }else{
                    line->set_freeze(true);
                }
                continue;
            }
        }
        
    }
}

nlohmann::json* ProjectServer::m_route_to_data(nlohmann::json* p_data, std::vector<PString>& p_keys, DefaultType p_type){
    nlohmann::json* data = p_data;
    size_t size = p_keys.size();
    size_t index = 1;
    for (FString key : p_keys) {
        if(index != size){
            if(!data->contains(key)){
                data->emplace(key, nlohmann::json());
                data = &(*data)[key];
            }else{
                if(!(*data)[key].is_object()){
                    ERROR_MSG("Project Data Path is not correct.");
                    return p_data;
                }else{
                    data = &(*data)[key];
                }
            }
        }else{
            if(!data->contains(key)){
                switch (p_type) {
                    case STRING:
                        (*data)[key] = "";
                        break;
                    case INT:
                        (*data)[key] = (int)0;
                        break;
                    case FLOAT:
                        (*data)[key] = 0.0f;
                        break;
                    case BOOL:
                        (*data)[key] = false;
                        break;
                    case ARRAY:
                        (*data)[key] = nlohmann::json::array();
                        break;
                }
                data = &(*data)[key];
            }else{
                bool is_type_correct = false;
                switch (p_type) {
                    case STRING:
                        is_type_correct = (*data)[key].is_string();
                        break;
                    case INT:
                        is_type_correct = (*data)[key].is_number_integer();
                        break;
                    case FLOAT:
                        is_type_correct = (*data)[key].is_number_float();
                        break;
                    case BOOL:
                        is_type_correct = (*data)[key].is_boolean();
                        break;
                    case ARRAY:
                        is_type_correct = (*data)[key].is_array();
                        break;
                }
                if(is_type_correct){
                    data = &(*data)[key];
                }else{
                    ERROR_MSG("Project Data Type is not correct.");
                    return p_data;
                }
            }
        }
        index++;
    }

    return data;
}
void ProjectServer::m_refresh_display_data(){
    std::vector<WorkspaceInfo>().swap(m_display_data);
    Project& project = m_projects[m_project_uid];
    for(auto& workspace : project.spaces){
        PString uid = workspace.first;
        Workspace& space = workspace.second;

        WorkspaceInfo info;
        info.uid = space.code;
        info.name = space.data["workspace_info"]["name"];
        info.path = ObjectServer::Ref()->get_instance<FStreamBase>(space.file_id)->get_path().string();
        info.size = ObjectServer::Ref()->get_instance<FStreamBase>(space.file_id)->get_size();
        info.sort_id = space.data["workspace_info"]["created_at"];
        info.is_selected = space.is_selected;
        info.is_saved = space.is_saved;

        m_display_data.push_back(info);
    }

    std::sort(m_display_data.begin(), m_display_data.end(), [](WorkspaceInfo& a, WorkspaceInfo& b){
        return a.sort_id < b.sort_id;
    });
}

void ProjectServer::set(PString p_project, PString p_workspace, std::vector<PString> p_keys, PString p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(PString p_project, PString p_workspace, std::vector<PString> p_keys, int p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(PString p_project, PString p_workspace, std::vector<PString> p_keys, float p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(PString p_project, PString p_workspace, PString p_key, bool p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}

void ProjectServer::set(PString p_project, PString p_workspace, PString p_key, PString p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(PString p_project, PString p_workspace, PString p_key, int p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(PString p_project, PString p_workspace, PString p_key, float p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(PString p_project, PString p_workspace, std::vector<PString> p_keys, bool p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}

void ProjectServer::set(ProjectPayload p_key, PString p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(ProjectPayload p_key, int p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(ProjectPayload p_key, float p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}
void ProjectServer::set(ProjectPayload p_key, bool p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    *data = p_val;
}

void ProjectServer::list_push_back(PString p_project, PString p_workspace, std::vector<PString> p_keys, PString p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(PString p_project, PString p_workspace, std::vector<PString> p_keys, int p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(PString p_project, PString p_workspace, std::vector<PString> p_keys, float p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(PString p_project, PString p_workspace, PString p_key, bool p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}

void ProjectServer::list_push_back(PString p_project, PString p_workspace, PString p_key, PString p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(PString p_project, PString p_workspace, PString p_key, int p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(PString p_project, PString p_workspace, PString p_key, float p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(PString p_project, PString p_workspace, std::vector<PString> p_keys, bool p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}

void ProjectServer::list_push_back(ProjectPayload p_key, PString p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(ProjectPayload p_key, int p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(ProjectPayload p_key, float p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}
void ProjectServer::list_push_back(ProjectPayload p_key, bool p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    if(data->size() > 0 && !(*data)[0].is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).push_back(p_val);
}

void ProjectServer::list_clear(PString p_project, PString p_workspace, std::vector<PString> p_keys){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }
    (*data).clear();
}
void ProjectServer::list_clear(PString p_project, PString p_workspace, PString p_key){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).clear();
}
void ProjectServer::list_clear(ProjectPayload p_key){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    m_projects[data_key.project].spaces[data_key.workspace].is_saved = false;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).clear();
}


void ProjectServer::get(PString p_project, PString p_workspace, PString p_key, PString& p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, PString p_key, int& p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, PString p_key, float& p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, PString p_key, bool& p_val){
    ProjectPayload data_key = {p_project, p_workspace, {p_key}};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, std::vector<PString> p_keys, PString& p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, std::vector<PString> p_keys, int& p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, std::vector<PString> p_keys, float& p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_project, PString p_workspace, std::vector<PString> p_keys, bool& p_val){
    ProjectPayload data_key = {p_project, p_workspace, p_keys};
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}

void ProjectServer::get(ProjectPayload p_key, PString& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(ProjectPayload p_key, int& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(ProjectPayload p_key, float& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(ProjectPayload p_key, bool& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}

void ProjectServer::remove_key(PString p_project, PString p_workspace, PString p_key){
    ProjectPayload data_key;
    data_key.project = p_project;
    data_key.workspace = p_workspace;
    data_key.keys.push_back(p_key);
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json* root = &m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = root;
    nlohmann::json* last_data = data;
    PString last_key;
    for(PString key : data_key.keys){
        last_data = data;
        last_key = key;
        if(!data->contains(key)){
            return;
        }
        
        data = &(*data)[key];
    }
    last_data->erase(last_key);
}
void ProjectServer::remove_key(PString p_project, PString p_workspace, std::vector<PString> p_keys){
    ProjectPayload data_key;
    data_key.project = p_project;
    data_key.workspace = p_workspace;
    data_key.keys = p_keys;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json* root = &m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = root;
    nlohmann::json* last_data = data;
    PString last_key;
    for(PString key : data_key.keys){
        last_data = data;
        last_key = key;
        if(!data->contains(key)){
            return;
        }
        
        data = &(*data)[key];
    }
    last_data->erase(last_key);
}
void ProjectServer::remove_key(ProjectPayload p_data_key){
    ProjectPayload& data_key = p_data_key;
    if(!data_key.is_project_exists()) return;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json* root = &m_projects[data_key.project].spaces[data_key.workspace].data;
    nlohmann::json* data = root;
    nlohmann::json* last_data = data;
    PString last_key;
    for(PString key : data_key.keys){
        last_data = data;
        last_key = key;
        if(!data->contains(key)){
            return;
        }
        
        data = &(*data)[key];
    }
    last_data->erase(last_key);
}


PString ProjectServer::current_project_uid(){
    return m_project_uid;
}
PString ProjectServer::current_workspace_uid(){
    return m_workspace_uid;
}
void ProjectServer::go_to_workspace(PString p_workspace){
    ProjectPayload key;
    key.project = m_project_uid;
    key.workspace = p_workspace;
    if(!key.is_workspace_exists()){
        return;
    }

    m_workspace_uid = p_workspace;
}

PString ProjectServer::create_workspace(){
    FString new_ws_uid = RandomCode(25).get();
    Workspace workspace;
    workspace.code = new_ws_uid;
    workspace.data["workspace_info"] = {};
    workspace.data["workspace_info"]["name"] = "New Project";
    workspace.data["workspace_info"]["created_at"] = std::chrono::utc_clock::now().time_since_epoch().count();
    workspace.data["workspace_info"]["updated_at"] = std::chrono::utc_clock::now().time_since_epoch().count();

    FStreamLink link;
    link.push_back("projects");
    link.push_back(m_project_uid);
    OID folder_id = FileServer::Ref()->get_fstream_obj(link);
    FStreamFolder* folder_ptr = ObjectServer::Ref()->get_instance<FStreamFolder>(folder_id);
    if(folder_ptr){
        workspace.file_id = folder_ptr->create_file(new_ws_uid);
        FStreamFile* file_ptr = ObjectServer::Ref()->get_instance<FStreamFile>(workspace.file_id);
        file_ptr->append(workspace.data.dump(4));
    }

    m_projects[m_project_uid].spaces.emplace(new_ws_uid, workspace);
    return new_ws_uid;
}
void ProjectServer::remove_workspace(PString p_workspace){
    ProjectPayload key;
    key.project = m_project_uid;
    key.workspace = p_workspace;
    if(!key.is_workspace_exists()){
        return;
    }

    m_projects[m_project_uid].spaces.erase(p_workspace);
}
size_t ProjectServer::get_workspace_count(){
    return m_projects[m_project_uid].spaces.size();
}
std::vector<ProjectServer::WorkspaceInfo> ProjectServer::get_display_data(){
    return m_display_data;
}

void ProjectServer::save_workspace(PString p_uid){
    Project& project = m_projects[m_project_uid];
    if(project.spaces.contains(p_uid)){
        project.spaces[p_uid].save();
    }
}
void ProjectServer::save_all_workspaces(){
    Project& project = m_projects[m_project_uid];
    for(auto& it : project.spaces){
        it.second.save();
    }
}