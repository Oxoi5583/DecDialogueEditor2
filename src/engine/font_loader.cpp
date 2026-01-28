#include "engine/font_loader.h"

#include "DecToolsBox/debug/messenger.h"
#include "imgui/imgui_internal.h"
#include "server/ui_text_bank.h"
#include "imgui/imgui.h"
#include <cstddef>
#include <map>


void EngineFontLoader::init(){
    m_build_font(FONT_SIZE_MIDDLE, 1.8f);
    m_build_font(FONT_SIZE_BIG, 1.8f);
    m_build_font(FONT_SIZE_SMALL, 1.8f);
}

bool EngineFontLoader::m_is_font_loaded(FontSize p_size, FontRasterizerMultiply p_rasterizer_multiply){
    if(m_fonts.contains(p_size)
    && m_fonts[p_size].contains(p_rasterizer_multiply)){
        return true;
    }else{
        return false;
    }
}
void EngineFontLoader::m_build_font(FontSize p_size, FontRasterizerMultiply p_rasterizer_multiply){
    if(m_is_font_loaded(p_size, p_rasterizer_multiply)){
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    
    ImFont* font_ptr = nullptr;
    for(size_t i = 0; i < m_font_paths.size(); i++){
        FontDataset& data = m_font_paths[i];
        std::string path = data.path;

        ImFontConfig cfg;
        cfg.RasterizerMultiply = p_rasterizer_multiply;
        cfg.OversampleH = 2;
        cfg.OversampleV = 2;
        cfg.SizePixels = p_size;
        cfg.MergeMode = (i > 0);
        cfg.PixelSnapH = true; 
        cfg.PixelSnapV = true;
        cfg.GlyphOffset.y = 1.0f;
    

        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(data.glyph_range);
        ImVector<ImWchar> unified_ranges;
        builder.BuildRanges(&unified_ranges);

        font_ptr = io.Fonts->AddFontFromFileTTF(path.c_str(), p_size, &cfg, unified_ranges.Data);
    }

    io.Fonts->Build();

    if(!m_fonts.contains(p_size)){
        m_fonts.emplace(p_size, std::map<FontRasterizerMultiply, ImFont*>());
    }

    m_fonts[p_size].emplace(p_rasterizer_multiply, font_ptr);
}

ImFont* EngineFontLoader::get(FontSize p_size, FontRasterizerMultiply p_rasterizer_multiply){
    if(!m_is_font_loaded(p_size, p_rasterizer_multiply)){
        m_build_font(p_size, p_rasterizer_multiply);
    }
    if(!m_is_font_loaded(p_size, p_rasterizer_multiply)){
        return nullptr;
    }

    return m_fonts[p_size][p_rasterizer_multiply];
}