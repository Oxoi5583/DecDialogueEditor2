#pragma once

#include "DecToolsBox/debug/messenger.h"
#include <DecToolsBox/abstract/singleton.h>
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <type_traits>

using namespace nlohmann;

class ConfigLoader : public Singleton<ConfigLoader>{
private:
    const char* m_setting_path = "config/setting.json";
    json m_setting_data;
    std::vector<std::string> m_setting_keys = {
        "UsingTheme"
    };

    bool m_is_first_load = true;

    bool m_load_setting();
    bool m_save_setting();
public:

    void load();
    void save();

    json get_config_obj();

    template<typename T>
    void get_config(std::string p_key ,T& p_value){
        if(!m_setting_data.contains(p_key)){
            ERROR_MSG("Config key " << p_key << " not found.");
            return;
        }
        json::value_type value = m_setting_data[p_key];
        if constexpr (std::is_same_v<T, int>) {
            if(value.is_number_integer()){
                p_value = value;
                return;
            }else{
                ERROR_MSG("Config key " << p_key << " : not int value.");
                return;
            }
        }
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            if(value.is_number_float()){
                p_value = value;
                return;
            }else{
                ERROR_MSG("Config key " << p_key << " : not float/double value.");
                return;
            }
        }
        if constexpr (std::is_same_v<T, std::string>) {
            if(value.is_string()){
                p_value = value;
                return;
            }else{
                ERROR_MSG("Config key " << p_key << " : not string value.");
                return;
            }
        }
        if constexpr (!(
            std::is_same_v<T, int> ||
            std::is_same_v<T, float> || 
            std::is_same_v<T, double> || 
            std::is_same_v<T, std::string>
        )) {
            ERROR_MSG("Config key " << p_key << " : unidentified value type.");
            return;
        }
    }
    template<typename T>
    void set_config(std::string p_key ,T& p_value){
        if(!m_setting_data.contains(p_key)){
            ERROR_MSG("Config key " << p_key << " not found.");
            return;
        }
        json::value_type value = m_setting_data[p_key];
        if constexpr (std::is_same_v<T, int>) {
            if(value.is_number_integer()){
                m_setting_data[p_key] = p_value;
                return;
            }else{
                ERROR_MSG("Config key " << p_key << " : not int value.");
                return;
            }
        }
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            if(value.is_number_float()){
                m_setting_data[p_key] = p_value;
                return;
            }else{
                ERROR_MSG("Config key " << p_key << " : not float/double value.");
                return;
            }
        }
        if constexpr (std::is_same_v<T, std::string>) {
            if(value.is_string()){
                m_setting_data[p_key] = p_value;
                return;
            }else{
                ERROR_MSG("Config key " << p_key << " : not string value.");
                return;
            }
        }
        if constexpr (!(
            std::is_same_v<T, int> ||
            std::is_same_v<T, float> || 
            std::is_same_v<T, double> || 
            std::is_same_v<T, std::string>
        )) {
            ERROR_MSG("Config key " << p_key << " : unidentified value type.");
            return;
        }
    }
};