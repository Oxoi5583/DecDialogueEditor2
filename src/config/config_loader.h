#pragma once

#include <DecToolsBox/abstract/singleton.h>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>

using namespace nlohmann;

class ConfigLoader : public Singleton<ConfigLoader>{
private:
    const char* m_setting_path = "config/setting.json";
    json m_setting_data;
    std::vector<std::string> m_setting_keys = {
        "UsingTheme"
    };

    bool m_load_setting();
    bool m_save_setting();
public:
    ConfigLoader();

    json get_config();
};