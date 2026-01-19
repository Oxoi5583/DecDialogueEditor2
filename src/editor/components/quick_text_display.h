#pragma once

#include "DecToolsBox/abstract./singleton.h"

class QuickTextDisplay : public Singleton<QuickTextDisplay>{
private:
    std::string m_text = "";

    const int m_max_display_length = 50;

    bool m_is_shown = true;
public:
    QuickTextDisplay();
    ~QuickTextDisplay();

    void pre_process();
    void process();

    void show();
    void hide();
    void set_text(std::string p_text);
};