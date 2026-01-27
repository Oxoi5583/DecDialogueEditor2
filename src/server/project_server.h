#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include <string>
#include <vector>

#include "server/file_server.h"

class ProjectServer : public Singleton<ProjectServer>{
private:
    std::string m_current_project = "temp";

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

public:
    ProjectServer();
    ~ProjectServer();

    FPathWrapper& get_project();
    void set_project(std::string p_name);
    void create_file();

    bool is_project_file_valid(nlohmann::json& p_data);

    const std::string default_name = "temp";
};