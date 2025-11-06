#include "engine/font_loader.h"

#include "DecToolsBox/debug/messenger.h"
#include "imgui/imgui.h"


void EngineFontLoader::load(){
    if(m_is_first_load){
        if(!m_load_fonts()){
            ERROR_MSG("Fonts first loading failed.");
            exit(-1);
        }else{
            SUCCESS_MSG("Fonts first loaded successfully.");
        }
        m_is_first_load = false;
    }else{
        if(!m_load_fonts()){
            ERROR_MSG("Fonts loading failed. Setting will not be changed.");
        }else{
            SUCCESS_MSG("Fonts loaded successfully.");
        }
    }
}
void EngineFontLoader::init(){
    load();
}

bool EngineFontLoader::m_load_fonts(){
    auto buffer = std::move(m_fonts);

    int error_found = 0;

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    for(auto font : m_fonts_data){
        ImFont* ptr = io.Fonts->AddFontFromFileTTF(font.file_name.c_str(), font.size);
        if(!ptr){
            error_found++;
            continue;
        }

        m_fonts.emplace(font.font_id, ptr);
    }

    if(error_found > 0){
        for(auto font : m_fonts){
            io.Fonts->RemoveFont(font.second);
        }
        m_fonts = std::move(buffer);
        return false;
    }else{
        for(auto font : buffer){
            io.Fonts->RemoveFont(font.second);
        }
        return true;
    }
}

ImFont* EngineFontLoader::get(FontId p_id){
    if(!m_fonts.contains(p_id)){
        return nullptr;
    }

    return m_fonts[p_id];
}