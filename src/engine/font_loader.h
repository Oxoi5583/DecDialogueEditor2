#pragma once

#include "glad/glad.h"
#include "DecToolsBox/abstract/singleton.h"
#include "imgui/imgui.h"
#include <map>
#include <filesystem>

#define FONT_PATH(ARG) std::filesystem::absolute(std::filesystem::path(ARG)).string()

class EngineFontLoader : public Singleton<EngineFontLoader>{
public:
    typedef float FontSize;
    typedef float FontRasterizerMultiply;

    ImFont* get(FontSize p_size = 18.0f, FontRasterizerMultiply p_rasterizer_multiply = 1.8f);

    void init();
private:
    struct FontDataset{
        std::string path;
        const ImWchar* glyph_range;
    };

    static constexpr ImWchar material_ranges[] = {
        0xE000, 0xF8FF,
        0
    };

    std::vector<FontDataset> m_font_paths = {
        {FONT_PATH("fonts/ui_font_cn.ttf"), ImGui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon()},
        {FONT_PATH("fonts/ui_font_zh.ttf"), ImGui::GetIO().Fonts->GetGlyphRangesChineseFull()},
        {FONT_PATH("fonts/ui_font_jp.ttf"), ImGui::GetIO().Fonts->GetGlyphRangesJapanese()},
        {FONT_PATH("fonts/ui_font_ko.ttf"), ImGui::GetIO().Fonts->GetGlyphRangesKorean()},
        {FONT_PATH("fonts/ui_font_en.ttf"), ImGui::GetIO().Fonts->GetGlyphRangesDefault()},
        {FONT_PATH("fonts/ui_icon.ttf"), material_ranges},
    };

    std::map<FontSize, std::map<FontRasterizerMultiply, ImFont*>> m_fonts;
    
    bool m_is_font_loaded(FontSize p_size, FontRasterizerMultiply p_rasterizer_multiply);
    void m_build_font(FontSize p_size, FontRasterizerMultiply p_rasterizer_multiply);

    bool m_is_first_load = true;
};