#include "engine/font_loader.h"

#include "DecToolsBox/debug/messenger.h"
#include "core/ui_text_bank.h"
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
    ImGuiIO& io = ImGui::GetIO();
    ImFontGlyphRangesBuilder builder;
    static const ImWchar ranges[] = {
        0x25A0, 0x25FF,
    };
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
    builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.AddRanges(io.Fonts->GetGlyphRangesThai());
    builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
    builder.AddRanges(ranges);
    ImVector<ImWchar> unified_ranges;
    builder.BuildRanges(&unified_ranges);

    auto buffer = std::move(m_fonts);

    int error_found = 0;
    
    for(auto font : m_fonts_data){
        ImFontConfig cfg;
        cfg.RasterizerMultiply = 1.8f;
        cfg.OversampleH = 2;
        cfg.OversampleV = 2;
        cfg.SizePixels = font.size;
        ImFont* ptr = io.Fonts->AddFontFromFileTTF(font.file_name.c_str(), font.size, &cfg, unified_ranges.Data);
        const void * address = static_cast<const void*>(ptr);
        std::stringstream ss;
        ss << address;  
        std::string name = ss.str(); 

        INFO_MSG("Loading " << font.font_id << " : " << font.file_name << " (" << font.size << ") to " << name);
        if(!ptr){
            error_found++;
            continue;
        }

        m_fonts.emplace(font.font_id, ptr);
    }
    io.Fonts->Build();

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