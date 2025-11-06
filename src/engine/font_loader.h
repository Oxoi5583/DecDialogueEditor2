#pragma once

#include "glad/glad.h"
#include "DecToolsBox/abstract/singleton.h"
#include "imgui/imgui.h"
#include <map>

class EngineFontLoader : public Singleton<EngineFontLoader>{
public:
    enum FontId{
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
        {UI_ICON_BIG ,"fonts/ui_icon.ttf",32.0f},
        {UI_ICON_MIDDLE ,"fonts/ui_icon.ttf",24.0f},
        {UI_ICON_SMALL ,"fonts/ui_icon.ttf",14.0f},
    };

    std::map<int, ImFont*> m_fonts;

    bool m_load_fonts();

    bool m_is_first_load = true;
};