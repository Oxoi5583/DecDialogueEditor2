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

    void save();
    void rename(std::string p_name);
    void edit(std::string p_key, std::string p_val);
    void edit(std::string p_key, int p_val);
    void edit(std::string p_key, float p_val);
    void edit(std::string p_key, bool p_val);
};

class ProjectServer : public Singleton<ProjectServer>{
public:
    typedef std::string ProjectID;
    typedef std::string WorkspaceID;
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

    std::map<ProjectID, std::map<WorkspaceID, ProjectWorkSpace>> m_projects;
    
    void m_scan_projects_folder();
    void m_freeze_graph_nodes_if_in_diff_workspace();
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

    std::map<WorkspaceID, ProjectWorkSpace>& get_project_data();
    std::vector<ProjectWorkSpace> get_project_data_sorted(bool p_is_asc = true);
    ProjectWorkSpace& get_workspace_data(std::string p_uid = "");
    std::string get_workspace_uid();

    void edit_workspace(std::string p_workspace_id, std::string p_key, std::string p_val);

    void save_workspace();
    void save_as_project();

    void init();
    void process();
};