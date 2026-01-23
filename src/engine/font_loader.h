#pragma once

#include "glad/glad.h"
#include "DecToolsBox/abstract/singleton.h"
#include "imgui/imgui.h"
#include <map>

class EngineFontLoader : public Singleton<EngineFontLoader>{
public:
    enum FontId{
        UI_FONT_BIG_EN,
        UI_FONT_MIDDLE_EN,
        UI_FONT_SMALL_EN,

        UI_FONT_BIG_ZH,
        UI_FONT_MIDDLE_ZH,
        UI_FONT_SMALL_ZH,

        UI_FONT_BIG_JP,
        UI_FONT_MIDDLE_JP,
        UI_FONT_SMALL_JP,

        UI_FONT_BIG_CN,
        UI_FONT_MIDDLE_CN,
        UI_FONT_SMALL_CN,

        UI_FONT_BIG_KO,
        UI_FONT_MIDDLE_KO,
        UI_FONT_SMALL_KO,

        UI_ICON_BIG,
        UI_ICON_MIDDLE,
        UI_ICON_SMALL,

        FONT_COUNT,
    };

    ImFont* get(FontId p_id);
    void load();
    void init();
private:
    struct FontData{
        int font_id;
        std::string file_name;
        float size;
    };

    const std::vector<FontData> m_fonts_data = {
        {UI_FONT_BIG_EN ,"fonts/ui_font_en.ttf",32.0f},
        {UI_FONT_MIDDLE_EN ,"fonts/ui_font_en.ttf",18.0f},
        {UI_FONT_SMALL_EN ,"fonts/ui_font_en.ttf",16.0f},

        {UI_FONT_BIG_ZH ,"fonts/ui_font_zh.ttf",32.0f},
        {UI_FONT_MIDDLE_ZH ,"fonts/ui_font_zh.ttf",18.0f},
        {UI_FONT_SMALL_ZH ,"fonts/ui_font_zh.ttf",16.0f},

        {UI_FONT_BIG_JP ,"fonts/ui_font_jp.ttf",32.0f},
        {UI_FONT_MIDDLE_JP ,"fonts/ui_font_jp.ttf",18.0f},
        {UI_FONT_SMALL_JP ,"fonts/ui_font_jp.ttf",16.0f},

        {UI_FONT_BIG_CN ,"fonts/ui_font_cn.ttf",32.0f},
        {UI_FONT_MIDDLE_CN ,"fonts/ui_font_cn.ttf",18.0f},
        {UI_FONT_SMALL_CN ,"fonts/ui_font_cn.ttf",16.0f},

        {UI_FONT_BIG_KO ,"fonts/ui_font_ko.ttf",32.0f},
        {UI_FONT_MIDDLE_KO ,"fonts/ui_font_ko.ttf",18.0f},
        {UI_FONT_SMALL_KO ,"fonts/ui_font_ko.ttf",16.0f},

        {UI_ICON_BIG ,"fonts/ui_icon.ttf",32.0f},
        {UI_ICON_MIDDLE ,"fonts/ui_icon.ttf",18.0f},
        {UI_ICON_SMALL ,"fonts/ui_icon.ttf",12.0f},
    };

    std::map<int, ImFont*> m_fonts;

    bool m_load_fonts();

    bool m_is_first_load = true;
};