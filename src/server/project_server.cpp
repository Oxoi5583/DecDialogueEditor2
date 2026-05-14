#include "server/project_server.h"
#include "DecToolsBox/core/random_code.h"
#include "DecToolsBox/debug/messenger.h"
#include "SimZip.h"
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
#include <editor/components/explorer_window.h>
#include <engine/window.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <nlohmann/json.hpp>
#include <server/event_server.h>
#include <server/events.h>
#include <server/object_base.h>
#include <string>
#include <system/graph/camera.h>
#include <unordered_map>
#include <vector>


bool ProjectPayload::is_workspace_exists(){
    return ProjectServer::Ref()->m_project.spaces.contains(this->workspace);
}
bool ProjectPayload::is_data_exists(){
    if(!is_workspace_exists()){
        return false;
    }
    nlohmann::json& root = ProjectServer::Ref()->m_project.spaces[this->workspace].data;
    for(PString key : this->keys){
        if(root.contains(key)){
            root = root[key];
        }else{
            return false;
        }
    }
    return true;
}

void Workspace::save(){
    GraphCamera::Ref()->upload_data_to_project_server();

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
    m_handle_action();
}
void ProjectServer::init(){
    FStreamLink link;
    link.push_back(m_temp_folder_name);
    m_project.folder_id = FileServer::Ref()->get_fstream_obj(link);

    PString ws_code = this->create_workspace();
    this->go_to_workspace(ws_code);
}

ProjectServer::~ProjectServer(){
}

void ProjectServer::m_update_workspace_selection(){
    for(auto& it : m_project.spaces){
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
    Project& project = m_project;
    for(auto& workspace : project.spaces){
        PString uid = workspace.first;
        Workspace& space = workspace.second;
        FStreamBase* file = ObjectServer::Ref()->get_instance<FStreamBase>(space.file_id);
        if(!file){
            continue;
        }

        WorkspaceInfo info;
        info.uid = space.code;
        info.name = space.data["workspace_info"]["name"];
        info.path = file->get_path().string();
        info.size = file->get_size();
        info.sort_id = space.data["workspace_info"]["created_at"];
        info.is_selected = space.is_selected;
        info.is_saved = space.is_saved;

        m_display_data.push_back(info);
    }

    std::sort(m_display_data.begin(), m_display_data.end(), [](WorkspaceInfo& a, WorkspaceInfo& b){
        return a.sort_id < b.sort_id;
    });
}

void ProjectServer::set(PString p_workspace, std::vector<PString> p_keys, PString p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }


    PString old_val = *data;
    PString new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(PString p_workspace, std::vector<PString> p_keys, int p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    int old_val = *data;
    int new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(PString p_workspace, std::vector<PString> p_keys, float p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    float old_val = *data;
    float new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(PString p_workspace, PString p_key, bool p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    bool old_val = *data;
    bool new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}

void ProjectServer::set(PString p_workspace, PString p_key, PString p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    PString old_val = *data;
    PString new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(PString p_workspace, PString p_key, int p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    int old_val = *data;
    int new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(PString p_workspace, PString p_key, float p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    float old_val = *data;
    float new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(PString p_workspace, std::vector<PString> p_keys, bool p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    bool old_val = *data;
    bool new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}

void ProjectServer::set(ProjectPayload p_key, PString p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    PString old_val = *data;
    PString new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(ProjectPayload p_key, int p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    int old_val = *data;
    int new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(ProjectPayload p_key, float p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    float old_val = *data;
    float new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}
void ProjectServer::set(ProjectPayload p_key, bool p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    bool old_val = *data;
    bool new_val = p_val;
    if(old_val != new_val){
        *data = p_val;
        m_project.spaces[data_key.workspace].is_saved = false;
        m_project.is_saved = false;
    }
}

void ProjectServer::list_push_back(PString p_workspace, std::vector<PString> p_keys, PString p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
void ProjectServer::list_push_back(PString p_workspace, std::vector<PString> p_keys, int p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
void ProjectServer::list_push_back(PString p_workspace, std::vector<PString> p_keys, float p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
void ProjectServer::list_push_back(PString p_workspace, PString p_key, bool p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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

void ProjectServer::list_push_back(PString p_workspace, PString p_key, PString p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
void ProjectServer::list_push_back(PString p_workspace, PString p_key, int p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
void ProjectServer::list_push_back(PString p_workspace, PString p_key, float p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
void ProjectServer::list_push_back(PString p_workspace, std::vector<PString> p_keys, bool p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
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

void ProjectServer::list_clear(PString p_workspace, std::vector<PString> p_keys){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }
    (*data).clear();
}
void ProjectServer::list_clear(PString p_workspace, PString p_key){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).clear();
}
void ProjectServer::list_clear(ProjectPayload p_key){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    m_project.spaces[data_key.workspace].is_saved = false;
    m_project.is_saved = false;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::ARRAY);
    if(!data->is_array()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    (*data).clear();
}


void ProjectServer::get(PString p_workspace, PString p_key, PString& p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, PString p_key, int& p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, PString p_key, float& p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, PString p_key, bool& p_val){
    ProjectPayload data_key = {p_workspace, {p_key}};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, std::vector<PString> p_keys, PString& p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, std::vector<PString> p_keys, int& p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, std::vector<PString> p_keys, float& p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(PString p_workspace, std::vector<PString> p_keys, bool& p_val){
    ProjectPayload data_key = {p_workspace, p_keys};
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data get with wrong data type.");
        return;
    }

    p_val = *data;
}

void ProjectServer::get(ProjectPayload p_key, PString& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::STRING);
    if(!data->is_string()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(ProjectPayload p_key, int& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::INT);
    if(!data->is_number_integer()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(ProjectPayload p_key, float& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::FLOAT);
    if(!data->is_number_float()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}
void ProjectServer::get(ProjectPayload p_key, bool& p_val){
    ProjectPayload& data_key = p_key;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json& root = m_project.spaces[data_key.workspace].data;
    nlohmann::json* data = m_route_to_data(&root, data_key.keys, DefaultType::BOOL);
    if(!data->is_boolean()){
        ERROR_MSG("Project Data edit with wrong data type.");
        return;
    }

    p_val = *data;
}

void ProjectServer::remove_key(PString p_workspace, PString p_key){
    ProjectPayload data_key;
    data_key.workspace = p_workspace;
    data_key.keys.push_back(p_key);
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json* root = &m_project.spaces[data_key.workspace].data;
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
void ProjectServer::remove_key(PString p_workspace, std::vector<PString> p_keys){
    ProjectPayload data_key;
    data_key.workspace = p_workspace;
    data_key.keys = p_keys;
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json* root = &m_project.spaces[data_key.workspace].data;
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
    if(!data_key.is_workspace_exists()) return;

    nlohmann::json* root = &m_project.spaces[data_key.workspace].data;
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


PString ProjectServer::current_workspace_uid(){
    return m_workspace_uid;
}
void ProjectServer::go_to_workspace(PString p_workspace){
    ProjectPayload key;
    key.workspace = p_workspace;
    if(!key.is_workspace_exists()){
        return;
    }

    m_workspace_uid = p_workspace;

    nlohmann::json& data = m_project.spaces[m_workspace_uid].data;

    vec2 size = GraphCamera::Ref()->get_zoomed_size();
    
    if(data.contains("camera")){
        nlohmann::json& cam_data = data["camera"];
        vec2 pos;
        float zoom;
        if(cam_data.contains("position")){
            pos = {(float)cam_data["position"]["x"], (float)cam_data["position"]["y"]};
        }else{
            pos = size / 2.0f;
        }
        if(cam_data.contains("zoom")){
            zoom = (float)cam_data["zoom"];
        }else{
            zoom = 1;
        }
        GraphCamera::Ref()->set_zoom(zoom);
        GraphCamera::Ref()->set_target(pos);
    }
}

PString ProjectServer::create_workspace(){
    FString new_ws_uid = RandomCode(25).get();
    Workspace workspace;
    workspace.code = new_ws_uid;
    workspace.data["workspace_info"] = {
        {"name", "New Workspace"},
        {"created_at", std::chrono::utc_clock::now().time_since_epoch().count()},
        {"updated_at", std::chrono::utc_clock::now().time_since_epoch().count()},
    };

    FStreamLink link;
    link.push_back(m_temp_folder_name);
    OID folder_id = FileServer::Ref()->get_fstream_obj(link);
    FStreamFolder* folder_ptr = ObjectServer::Ref()->get_instance<FStreamFolder>(folder_id);
    if(folder_ptr){
        workspace.file_id = folder_ptr->create_file(new_ws_uid);
        FStreamFile* file_ptr = ObjectServer::Ref()->get_instance<FStreamFile>(workspace.file_id);
        file_ptr->append(workspace.data.dump(4));
        //file_ptr->locked();
    }

    m_project.spaces.emplace(new_ws_uid, workspace);
    return new_ws_uid;
}
void ProjectServer::remove_workspace(PString p_workspace){
    ProjectPayload key;
    key.workspace = p_workspace;
    if(!key.is_workspace_exists()){
        return;
    }

    m_project.spaces.erase(p_workspace);
}
size_t ProjectServer::get_workspace_count(){
    return m_project.spaces.size();
}
std::vector<ProjectServer::WorkspaceInfo> ProjectServer::get_display_data(){
    return m_display_data;
}

bool ProjectServer::m_action_open_project(FPath p_path){
    if(!std::filesystem::exists(p_path)){
        return false;
    }

    FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(m_project.folder_id);
    if(folder){
        folder->clear();
        folder->extract_from(p_path.string());
        m_project.spaces.clear();
        GraphManager::Ref()->clear_nodes();

        folder->extract_from(p_path.string());

        for(OID id : folder->get_children()){
            m_read_workspace(id);
        }

        m_refresh_display_data();
        WorkspaceInfo& info = m_display_data[0];
        this->go_to_workspace(info.uid);

        m_project_file = p_path;

        forced_switch_to_saved(2);
        return true;
    }
    return false;
}

bool ProjectServer::m_action_save_as_project(FPath p_path){
    std::streambuf* old_buf = std::cout.rdbuf();
    std::cout.rdbuf(nullptr); 
    try {
        if(FileServer::Ref()->is_in_programme_folder(p_path)){
            std::cout.rdbuf(old_buf);
            ERROR_MSG("Please save project outside of the programme file.");
            return false;
        }

        this->save_all_workspaces();
        p_path = std::filesystem::absolute(p_path);
        SimZip zip(p_path.string(), SimZip::OpenMode::Create);
        for(auto& it : m_project.spaces){
            FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(it.second.file_id);
            file->unlocked();
            if(file){
                zip.add(file->get_path().string());
            }
            file->locked();
        }
        zip.save();
        zip.close();

        forced_switch_to_saved();

        m_project_file = p_path;
        std::cout.rdbuf(old_buf);
        SUCCESS_MSG("Saved project to " << p_path.string() << " successfully");
        return true;
    } catch(std::exception e) {
        std::cout.rdbuf(old_buf);
        ERROR_MSG("Saving project to " << p_path.string() << " failed.");
        ERROR_MSG("Error msg : " << e.what());
    }

    std::cout.rdbuf(old_buf);
    return false;
}
bool ProjectServer::m_action_export_project(FPath p_path){
    std::streambuf* old_buf = std::cout.rdbuf();
    std::cout.rdbuf(nullptr);
    try {
        if(FileServer::Ref()->is_in_programme_folder(p_path)){
            std::cout.rdbuf(old_buf);
            ERROR_MSG("Please export project outside of the programme file.");
            return false;
        }

        this->save_all_workspaces();
        p_path = std::filesystem::absolute(p_path);

        basic_json<> export_data = {{"nodes", {}}};
        basic_json<> name_to_code = {};

        for(auto& it : m_project.spaces){
            Workspace& workspace = it.second;
            if(!workspace.data.contains("objects")){
                continue;
            }

            auto& ws_data = workspace.data["objects"];
            for(auto& it : ws_data.items()){
                auto& obj_code = it.key();
                auto& obj_data = it.value();
                auto& obj_name = obj_data["name"];

                if(name_to_code.contains(obj_name)){
                    continue;
                }
                
                basic_json<> export_obj_data = {
                    {"children", obj_data["children"]},
                    {"properties", obj_data["properties"]},
                };

                name_to_code.emplace(obj_name, obj_code);
                export_data["nodes"].emplace(obj_code, export_obj_data);
            }
        }

        export_data.emplace("mapping", name_to_code);
        
        std::ofstream f(p_path);
        f << std::setw(4) << export_data << std::endl;
        f.close();

        std::cout.rdbuf(old_buf);
        SUCCESS_MSG("Exported project to " << p_path.string() << " successfully");
        return true;
    } catch(std::exception e) {
        std::cout.rdbuf(old_buf);
        ERROR_MSG("Exporting project to " << p_path.string() << " failed.");
        ERROR_MSG("Error msg : " << e.what());
    }
    std::cout.rdbuf(old_buf);
    return false;
}

void ProjectServer::save_project(){
    if(m_project_action_packages.empty()){
        m_project_action_packages.push({ProjectAction::SAVE, 0});
    }
}
void ProjectServer::save_as_project(){
    if(m_project_action_packages.empty()){
        m_project_action_packages.push({ProjectAction::SAVE_AS, 0});
    }
}
void ProjectServer::open_project(){
    if(m_project_action_packages.empty()){
        m_project_action_packages.push({ProjectAction::OPEN, 0});
    }
}

void ProjectServer::save_workspace(PString p_uid){
    Project& project = m_project;
    if(project.spaces.contains(p_uid)){
        project.spaces[p_uid].save();
    }
}

void ProjectServer::save_all_workspaces(){
    Project& project = m_project;
    for(auto& it : project.spaces){
        it.second.save();
    }
}
void ProjectServer::export_project(){
    if(m_project_action_packages.empty()){
        m_project_action_packages.push({ProjectAction::EXPORT, 0});
    }
}


bool ProjectServer::is_workspace_file_valid(OID p_file){
    FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(p_file);
    if(!file){
        return false;
    }
    if(!file->is_exists()){
        return false;
    }
    if(m_project.spaces.contains(file->get_name())){
        return false;
    }
    try{
        nlohmann::json data = nlohmann::json::parse(file->read());
        if(!data.contains("workspace_info")){
            return false;
        }
        if(!data["workspace_info"].contains("name")){
            return false;
        }
        if(!data["workspace_info"].contains("created_at")){
            return false;
        }
        if(!data["workspace_info"].contains("updated_at")){
            return false;
        }
    }
    catch (const nlohmann::json::parse_error&){
        return false;
    }
    return true;
}

void ProjectServer::m_read_workspace(OID p_file){
    if(!is_workspace_file_valid(p_file)){
        return;
    }

    FStreamFile* file = ObjectServer::Ref()->get_instance<FStreamFile>(p_file);
    nlohmann::json data = nlohmann::json::parse(file->read());

    Workspace workspace;
    workspace.code = file->get_name();
    workspace.data = data;
    workspace.file_id = p_file;
    workspace.is_saved = true;
    workspace.is_selected = false;

    //file->locked();
    m_project.spaces.emplace(workspace.code, workspace);

    if(data.contains("camera")){
        if(data.contains("position")) GraphCamera::Ref()->set_target({data["position"]["x"], data["position"]["y"]});
        if(data.contains("zoom")) GraphCamera::Ref()->set_zoom(data["zoom"]);
    }else{
        GraphCamera::Ref()->set_target({0, 0});
        GraphCamera::Ref()->set_zoom(1);
    }

    if(data.contains("objects")){
        for(auto& [key, val] : data["objects"].items()){
            FString code = key;
            nlohmann::json obj_data = val;

            {
                EventSpawnNode event;
                event.spawn_pos = vec2(obj_data["position"]["x"],obj_data["position"]["y"]);
                event.type = GraphManager::Ref()->name_to_type(obj_data["type"]);
                event.is_workspace_custom = true;
                event.custom_workspace = workspace.code;
                event.is_uid_custom = true;
                event.custom_uid = code;
                event.is_name_custom = true;
                event.custom_name = obj_data["name"];

                if(obj_data.contains("properties")){
                    for(auto& obj_data : obj_data["properties"].items()){
                        std::string key = obj_data.key();
                        std::string val = obj_data.value()["value"];
                        int max_size = obj_data.value()["max_size"];

                        event.init_data.push_back({key, val, max_size});
                    }
                }

                EventServer::Ref()->emit(event);
            }

            {
                for(std::string to_uid : obj_data["children"]){
                    EventCreateConnectionWithUID event;
                    event.fm_uid = code;
                    event.to_uid = to_uid;
                    
                    EventServer::Ref()->emit(event);
                }
            }
        }
    }
}

bool ProjectServer::have_saved_target(){
    return std::filesystem::exists(m_project_file);
}
bool ProjectServer::have_explorer_window(){
    return ObjectServer::Ref()->is_id_valid(m_explorer_window_id);
}

void ProjectServer::m_handle_action(){
    if(m_project_action_packages.empty()){
        return;
    }

    bool is_list_need_to_clear = false;

    ProjectActionPackage& package = m_project_action_packages.front();

    if(package.delay_frame > 0){
        package.delay_frame--;
        return;
    }

    switch (package.action) {
        case ProjectAction::SAVE:{
            if(m_handle_action_save()){
                is_list_need_to_clear = true;
            }
            break;
        }
        case ProjectAction::SAVE_AS:{
            if(m_handle_action_save_as()){
                is_list_need_to_clear = true;
            }
            break;
        }
        case ProjectAction::OPEN:{
            if(m_handle_action_open()){
                is_list_need_to_clear = true;
            }
            break;
        }
        case ProjectAction::EXPORT:{
            if(m_handle_action_export()){
                is_list_need_to_clear = true;
            }
            break;
        }
        case ProjectAction::FORCE_TO_SAVE_STATUS:{
            this->save_all_workspaces();
            m_project.is_saved = true;
            is_list_need_to_clear = true;
            break;
        }
    }
    
    if(is_list_need_to_clear){
        m_project_action_packages.pop();
    }
}
OID ProjectServer::m_new_explorer_export(){
    ExplorerWindow* window = EngineWindow::Ref()->create_explorer_window();
    window->set_allow_multi_select(false);
    window->set_mode(ExplorerWindow::Mode::SAVE);
    window->add_filter(ExplorerWindow::FilterOption::DEC_DIALOGUE);
    window->set_default_path("dialogue_data.json");

    return window->get_id();
}
OID ProjectServer::m_new_explorer_save(){
    ExplorerWindow* window = EngineWindow::Ref()->create_explorer_window();
    window->set_allow_multi_select(false);
    window->set_mode(ExplorerWindow::Mode::SAVE);
    window->add_filter(ExplorerWindow::FilterOption::DEC_DIALOGUE);
    window->set_default_path("dialogue_project.ddlg");

    return window->get_id();
}
OID ProjectServer::m_new_explorer_open(){
    ExplorerWindow* window = EngineWindow::Ref()->create_explorer_window();
    window->set_allow_multi_select(false);
    window->set_mode(ExplorerWindow::Mode::FILE);
    window->add_filter(ExplorerWindow::FilterOption::DEC_DIALOGUE);
    window->set_default_path("dialogue_project.ddlg");

    return window->get_id();
}
bool ProjectServer::m_handle_action_save(){
    if(have_saved_target()){
        return m_action_save_as_project(m_project_file);
    }

    if(!have_explorer_window() && !have_saved_target()){
        m_explorer_window_id = m_new_explorer_save();
    }
    
    ExplorerWindow* window = ObjectServer::Ref()->get_instance<ExplorerWindow>(m_explorer_window_id);
    if(window->have_result()){
        if(window->get_result().empty()){
            window->close();
            return false;
        }

        std::string path = window->get_result()[0];
        if(!m_action_save_as_project(path)){
            window->close();
            return  false;
        }

        window->close();
        return true;
    }

    if(window->is_finished()){
        window->close();
        return true;
    }

    return false;
}
bool ProjectServer::m_handle_action_save_as(){
    if(!have_explorer_window()){
        m_explorer_window_id = m_new_explorer_save();
    }
    
    ExplorerWindow* window = ObjectServer::Ref()->get_instance<ExplorerWindow>(m_explorer_window_id);
    if(window->have_result()){
        if(window->get_result().empty()){
            window->close();
            return false;
        }

        std::string path = window->get_result()[0];
        if(!m_action_save_as_project(path)){
            window->close();
            return  false;
        }

        window->close();
        return true;
    }

    if(window->is_finished()){
        window->close();
        return true;
    }

    return false;
}
bool ProjectServer::m_handle_action_open(){
    if(!have_explorer_window()){
        m_explorer_window_id = m_new_explorer_open();
    }
    
    ExplorerWindow* window = ObjectServer::Ref()->get_instance<ExplorerWindow>(m_explorer_window_id);
    if(window->have_result()){
        if(window->get_result().empty()){
            window->close();
            return false;
        }

        std::string path = window->get_result()[0];
        if(!m_action_open_project(path)){
            window->close();
            return  false;
        }

        window->close();
        return true;
    }

    if(window->is_finished()){
        window->close();
        return true;
    }

    return false;
}
bool ProjectServer::m_handle_action_export(){
    if(!have_explorer_window()){
        m_explorer_window_id = m_new_explorer_export();
    }
    
    ExplorerWindow* window = ObjectServer::Ref()->get_instance<ExplorerWindow>(m_explorer_window_id);
    if(window->have_result()){
        if(window->get_result().empty()){
            window->close();
            return false;
        }

        std::string path = window->get_result()[0];
        if(!m_action_export_project(path)){
            window->close();
            return  false;
        }

        window->close();
        return true;
    }

    if(window->is_finished()){
        window->close();
        return true;
    }

    return false;
}


void ProjectServer::shutdown(){
    FStreamFolder* folder = ObjectServer::Ref()->get_instance<FStreamFolder>(m_project.folder_id);
    folder->clear();
}

bool ProjectServer::has_any_unsaved(){
    for(auto& it : m_project.spaces){
        Workspace& ws = it.second;
        if(!ws.is_saved){
            return true;
        }
    }

    if(!m_project.is_saved){
        return true;
    }

    return false;
}

void ProjectServer::forced_switch_to_saved(uint8_t p_delay_frame){
    this->m_project_action_packages.push({ ProjectAction::FORCE_TO_SAVE_STATUS, p_delay_frame});
}