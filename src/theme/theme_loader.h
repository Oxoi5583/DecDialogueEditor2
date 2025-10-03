#pragma once

#include <DecToolsBox/abstract/singleton.h>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using namespace nlohmann;

struct Color{
    float r;
    float g;
    float b;
    float a;
};

class ThemeLoader : public Singleton<ThemeLoader>{
private:
    const char* m_theme_folder = "theme";
    const char* m_default_theme_file = "theme/default.json";
    std::vector<std::string> m_theme_files;
    
    std::vector<std::string> m_theme_keys = {    
        "Name",
        "BrandColour",
        "SecondaryColour1",
        "SecondaryColour2" ,
        "SecondaryColour3",
        "AccentColour1",
        "AccentColour2"
    };

    std::map<std::string ,json> m_themes_json_objs;

    bool m_is_first_load = true;

    void m_load_all_theme_files();    
    bool m_is_theme_folder_exists();
    bool m_is_default_theme_exists();

    struct CheckResult{
        json data;
        bool is_valid;
    };
    CheckResult m_check_theme_file_valid(std::string p_path);
public:
    void load();

    std::string get_color_string(std::string p_key);
    Color get_color(std::string p_key);
};