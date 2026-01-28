#pragma once

#include "DecToolsBox/abstract./singleton.h"
#include "server/ui_text_bank.h"

class QuickTextDisplay : public Singleton<QuickTextDisplay>{
private:
    std::string m_text = "";

    const int m_max_display_length = 50;

    bool m_is_shown = true;
    float m_font_size = FONT_SIZE_MIDDLE;
public:
    QuickTextDisplay();
    ~QuickTextDisplay();

    void pre_process();
    void process();

    void show();
    void hide();
    void set_text(std::string p_text);
    void set_font_size(float p_size);
};