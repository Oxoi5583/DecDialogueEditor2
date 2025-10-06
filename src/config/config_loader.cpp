#include "config/config_loader.h"
#include "DecToolsBox/debug/messenger.h"
#include <exception>

void ConfigLoader::load(){
    if(m_is_first_load){
        if(!m_load_setting()){
            ERROR_MSG("Setting JSON first loading failed : " << m_setting_path);
            exit(-1);
        }else{
            SUCCESS_MSG("Setting JSON first loaded successfully : " << m_setting_path);
        }
        m_is_first_load = false;
    }else{
        if(!m_load_setting()){
            ERROR_MSG("Setting JSON loading failed. Setting will not be changed : " << m_setting_path);
        }else{
            SUCCESS_MSG("Setting JSON loaded successfully : " << m_setting_path);
        }
    }
}
void ConfigLoader::save(){
    if(!m_is_first_load){
        if(!m_save_setting()){
            ERROR_MSG("Setting JSON saving failed. Setting will not be changed : " << m_setting_path);
        }else{
            SUCCESS_MSG("Setting JSON saving successfully : " << m_setting_path);
        }
    }
}

bool ConfigLoader::m_load_setting(){
    std::ifstream f(m_setting_path);

    int error_found = 0;

    nlohmann::json data = {};

    try {
        data = nlohmann::json::parse(f);
    } catch (nlohmann::json::exception &e) {
        ERROR_MSG("Setting JSON parse failed : " << m_setting_path);
        error_found++;
    }

    for(auto& key : m_setting_keys){
        if(!data.contains(key)){
            ERROR_MSG("Setting JSON key" << key << " not found : " << m_setting_path);
            error_found++;
        }
    }

    if(error_found == 0){
        m_setting_data = data;
        return true;
    }else{
        return false;
    }
}
bool ConfigLoader::m_save_setting(){
    try {
        std::ofstream f(m_setting_path);
        f << std::setw(4) << m_setting_data << std::endl;
        return true;
    } catch (std::exception) {
        return false;
    }
}

nlohmann::json ConfigLoader::get_config_obj(){
    return m_setting_data;
}

