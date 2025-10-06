#include "theme/theme_loader.h"
#include "DecToolsBox/debug/messenger.h"
#include "config/config_loader.h"
#include "imgui/imgui.h"
#include "nlohmann/json.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>

bool ThemeLoader::m_is_theme_folder_exists(){
    return std::filesystem::is_directory(m_theme_folder);
}
bool ThemeLoader::m_is_default_theme_exists(){
    return std::filesystem::exists(m_default_theme_file);
}
bool ThemeLoader::m_is_default_theme_valid(){
    return m_check_theme_file_valid(m_default_theme_file).is_valid;
}
ThemeLoader::CheckResult ThemeLoader::m_check_theme_file_valid(std::string p_path){
    std::ifstream f(p_path);

    int error_found = 0;

    nlohmann::json data = {};

    try {
        data = nlohmann::json::parse(f);
    } catch (nlohmann::json::exception &e) {
        ERROR_MSG("Theme JSON parse failed : " << p_path);
        error_found++;
    }

    for(auto& key : m_theme_keys){
        if(!data.contains(key)){
            ERROR_MSG("Theme JSON key" << key << " not found : " << p_path);
            error_found++;
        }else{
            if(key != "Name"){
                std::string value = data[key];
                if(value.length() == 6 || value.length() == 8){
                    int illegal_char_count = 0;
                    for(char c : value){
                        if(!m_acceptable_char.contains(c)){
                            illegal_char_count++;
                        }
                    }
                    if(illegal_char_count > 0){
                        ERROR_MSG("Theme JSON value " << value << " of key " << key << " is illegal (Unacceptable Character) : " << p_path);
                        error_found++;
                    }
                }else{
                    ERROR_MSG("Theme JSON value " << value << " of key " << key << " is illegal (Wrong Length) : " << p_path);
                    error_found++;
                }
            }
        }
    }

    if(error_found == 0){
        return {data,true};
    }else{
        return {data,false};
    }
}
void ThemeLoader::m_load_all_theme_files(){
    for (auto &p : std::filesystem::recursive_directory_iterator(m_theme_folder)){
        if (p.path().extension() == ".json"){
            m_theme_files.push_back(p.path().relative_path().string());
            INFO_MSG("Theme JSON added to load-ready list : " << p.path().relative_path().string());
        }
    }
    for(auto &f : m_theme_files){
        INFO_MSG("Theme JSON being checked : " << f);
        CheckResult result = m_check_theme_file_valid(f);
        if(result.is_valid){
            std::string data_name = result.data["Name"];
            result.data.erase("Name");
            m_themes_json_objs.emplace(data_name, result.data);
            SUCCESS_MSG("Theme JSON loaded successfully : " << f);
        }else{
            ERROR_MSG("Theme JSON rejceted : " << f);
        }
    }
    INFO_MSG("All theme JSON finished loading");
}


void ThemeLoader::load(){
    if(!m_is_theme_folder_exists()){
        ERROR_MSG("Theme Loading Error : No theme folder found.");
        if(m_is_first_load){
            exit(-1);
        }
        return;
    }
    if(!m_is_default_theme_exists()){
        ERROR_MSG("Theme Loading Error : No default theme file found.");
        if(m_is_first_load){
            exit(-1);
        }
        return;
    }
    if(!m_is_default_theme_valid()){
        ERROR_MSG("Theme Loading Error : default theme is invalid to load");
        if(m_is_first_load){
            exit(-1);
        }
        return;
    }

    m_load_all_theme_files();

    if(m_is_first_load){
        m_is_first_load = false;
    }
}



std::string ThemeLoader::get_color_string(std::string p_key){
    std::string using_theme_name;
    ConfigLoader::Ref()->get_config("UsingTheme", using_theme_name);
    json theme_data = m_themes_json_objs[using_theme_name];

    if(theme_data.contains(p_key)){
        return theme_data[p_key];
    }else{
        return "000000";
    }
}
ImVec4 ThemeLoader::get_color(std::string p_key){
    std::string color_string = get_color_string(p_key);
    ImVec4 ret;
    if(color_string.length() == 8){
        std::string r_str = color_string.substr(0, 2);
        std::string g_str = color_string.substr(2, 2);
        std::string b_str = color_string.substr(4, 2);
        std::string a_str = color_string.substr(6, 2);

        ret.x = (float)std::stoi(r_str, nullptr, 16) / 255.0;
        ret.y = (float)std::stoi(g_str, nullptr, 16) / 255.0;
        ret.z = (float)std::stoi(b_str, nullptr, 16) / 255.0;
        ret.w = (float)std::stoi(a_str, nullptr, 16) / 255.0;
    }
    if(color_string.length() == 6){
        std::string r_str = color_string.substr(0, 2);
        std::string g_str = color_string.substr(2, 2);
        std::string b_str = color_string.substr(4, 2);

        ret.x = (float)std::stoi(r_str, nullptr, 16) / 255.0;
        ret.y = (float)std::stoi(g_str, nullptr, 16) / 255.0;
        ret.z = (float)std::stoi(b_str, nullptr, 16) / 255.0;
        ret.w = 1;
    }
    return ret;
}