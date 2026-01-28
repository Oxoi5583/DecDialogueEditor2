#pragma once

#include "DecToolsBox/abstract./singleton.h"

class StartupPopup : public Singleton<StartupPopup>{
private:
    bool m_ran = true;
public:
    StartupPopup() = default;
    ~StartupPopup() = default;

    void process();
};