#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "file_server.h"
#include "server/file_server.h"
#include <cstddef>
#include <cstdint>
#include <editor/components/explorer_window.h>
#include <map>
#include <queue>
#include <string>
#include <vector>

struct Project;
struct Workspace;

typedef std::string PString;
typedef std::string ProjectCode;
typedef std::string WorkspaceCode;
typedef std::map<PString, Workspace> Workspaces;


struct Project{
    OID folder_id;
    Workspaces spaces;
};

struct Workspace{
    WorkspaceCode code;
    OID file_id;
    nlohmann::json data;
    bool is_saved;
    bool is_selected;

    void save();
};

struct ProjectPayload{
    PString workspace;
    std::vector<PString> keys;
    
    bool is_workspace_exists();
    bool is_data_exists();
};

class ProjectServer : public Singleton<ProjectServer>{
public:
    struct WorkspaceInfo{
        PString uid;
        PString name;
        PString path;
        FSizeUnit size;
        bool is_saved;
        bool is_selected;
        uintmax_t sort_id;
    };
private:
    Project m_project;

    enum DefaultType{
        STRING,
        INT,
        FLOAT,
        BOOL,
        ARRAY,
    };
    nlohmann::json* m_route_to_data(nlohmann::json* p_data, std::vector<PString>& p_keys, DefaultType p_type);

    std::vector<WorkspaceInfo> m_display_data;
    void m_refresh_display_data();

    PString m_temp_folder_name = ".temp";
    PString m_workspace_uid;

    void m_update_workspace_selection();
    void m_freeze_non_workspace_obj();

    void m_read_workspace(OID p_file);

    FPath m_project_file = "";
    OID m_explorer_window_id = 0;

    bool m_action_save_as_project(FPath p_path);
    bool m_action_open_project(FPath p_path);

    enum class ProjectAction{
        SAVE,
        SAVE_AS,
        OPEN,
        EXPORT,
    };
    struct ProjectActionPackage{
        ProjectAction action;
    };
    std::vector<ProjectActionPackage> m_project_action_packages;
    void m_handle_action();
    bool m_handle_action_save();
    bool m_handle_action_save_as();
    bool m_handle_action_open();

    OID m_new_explorer_save();
    OID m_new_explorer_open();
public:
    ProjectServer() = default;
    ~ProjectServer();
    void process();
    void init();

    void set(PString p_workspace, PString p_key, PString p_val);
    void set(PString p_workspace, PString p_key, int p_val);
    void set(PString p_workspace, PString p_key, float p_val);
    void set(PString p_workspace, PString p_key, bool p_val);

    void set(PString p_workspace, std::vector<PString> p_keys, PString p_val);
    void set(PString p_workspace, std::vector<PString> p_keys, int p_val);
    void set(PString p_workspace, std::vector<PString> p_keys, float p_val);
    void set(PString p_workspace, std::vector<PString> p_keys, bool p_val);
    
    void set(ProjectPayload p_key, PString p_val);
    void set(ProjectPayload p_key, int p_val);
    void set(ProjectPayload p_key, float p_val);
    void set(ProjectPayload p_key, bool p_val);

    void list_push_back(PString p_workspace, PString p_key, PString p_val);
    void list_push_back(PString p_workspace, PString p_key, int p_val);
    void list_push_back(PString p_workspace, PString p_key, float p_val);
    void list_push_back(PString p_workspace, PString p_key, bool p_val);

    void list_push_back(PString p_workspace, std::vector<PString> p_keys, PString p_val);
    void list_push_back(PString p_workspace, std::vector<PString> p_keys, int p_val);
    void list_push_back(PString p_workspace, std::vector<PString> p_keys, float p_val);
    void list_push_back(PString p_workspace, std::vector<PString> p_keys, bool p_val);
    
    void list_push_back(ProjectPayload p_key, PString p_val);
    void list_push_back(ProjectPayload p_key, int p_val);
    void list_push_back(ProjectPayload p_key, float p_val);
    void list_push_back(ProjectPayload p_key, bool p_val);

    void list_clear(PString p_workspace, PString p_key);
    void list_clear(PString p_workspace, std::vector<PString> p_keys);
    void list_clear(ProjectPayload p_key);

    void get(PString p_workspace, PString p_key, PString& p_val);
    void get(PString p_workspace, PString p_key, int& p_val);
    void get(PString p_workspace, PString p_key, float& p_val);
    void get(PString p_workspace, PString p_key, bool& p_val);

    void get(PString p_workspace, std::vector<PString> p_keys, PString& p_val);
    void get(PString p_workspace, std::vector<PString> p_keys, int& p_val);
    void get(PString p_workspace, std::vector<PString> p_keys, float& p_val);
    void get(PString p_workspace, std::vector<PString> p_keys, bool& p_val);
    
    void get(ProjectPayload p_key, PString& p_val);
    void get(ProjectPayload p_key, int& p_val);
    void get(ProjectPayload p_key, float& p_val);
    void get(ProjectPayload p_key, bool& p_val);

    void remove_key(PString p_workspace, PString p_key);
    void remove_key(PString p_workspace, std::vector<PString> p_keys);
    void remove_key(ProjectPayload p_data_key);

    PString current_workspace_uid();
    void go_to_workspace(PString p_workspace);

    PString create_workspace();
    void remove_workspace(PString p_workspace);

    std::vector<WorkspaceInfo> get_display_data();
    size_t get_workspace_count();

    void open_project();
    void save_project();
    void save_as_project();
    void save_workspace(PString p_uid);
    void save_all_workspaces();
    void export_project();

    bool have_saved_target();
    bool have_explorer_window();

    bool is_workspace_file_valid(OID p_file);

    void shutdown();

    friend struct ProjectPayload;
};