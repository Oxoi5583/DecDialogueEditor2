#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "server/file_server.h"

struct ProjectWorkSpace{
    long long load_pri;
    std::string name;
    std::string path;
    std::string uid;
    FPathWrapper* path_wrapper;
    FSizeUnit size;
    nlohmann::json data;
    bool is_selected;
};

class ProjectServer : public Singleton<ProjectServer>{
private:
    std::string m_current_project = "temp";
    std::string m_current_workspace = "temp";

    void m_clear_files_if_not_saved();
    
    enum JSONValueType{
        STRING,
        NUMERIC,
        BOOLEAN,
        ARRAY,
        DICTIONARY,
    };

    struct JSONCheck{
        std::string field;
        JSONValueType type;
    };

    std::vector<JSONCheck> m_json_checklist = {
        {"name", JSONValueType::STRING},
    };

    std::map<std::string, std::map<std::string, ProjectWorkSpace>> m_projects;
    void m_refresh_projects_data();

    void m_freeze_graph_nodes_if_in_workspace();
    void m_free_graph_nodes_if_workspace_not_exists();
public:
    ProjectServer();
    ~ProjectServer();

    FPathWrapper& get_project_root();
    void set_project(std::string p_name);
    void set_workspace(std::string p_uid);


    FString create_file();

    bool is_project_file_valid(nlohmann::json& p_data);

    const std::string default_name = "temp";

    std::map<std::string, ProjectWorkSpace> get_project_data();
    ProjectWorkSpace get_workspace_data();
    std::string get_workspace_uid();

    void save_workspace();

    void process();
};