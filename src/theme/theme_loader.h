#pragma once

#include <DecToolsBox/abstract/singleton.h>
#include <set>
#include <string>
#include <vector>

#include "imgui/imgui.h"
#include "nlohmann/json.hpp"

using namespace nlohmann;


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

    std::set<char> m_acceptable_char = {
        '0','1','2','3',
        '4','5','6','7',
        '8','9','a','b',
        'c','d','e','f',
        'A','B','C','D',
        'E','F'
    };

    std::map<std::string ,json> m_themes_json_objs;

    bool m_is_first_load = true;

    void m_load_all_theme_files();    
    bool m_is_theme_folder_exists();
    bool m_is_default_theme_exists();
    bool m_is_default_theme_valid();

    struct CheckResult{
        json data;
        bool is_valid;
    };
    CheckResult m_check_theme_file_valid(std::string p_path);
public:
    void load();

    std::string get_color_string(std::string p_key);
    ImVec4 get_color(std::string p_key);
};