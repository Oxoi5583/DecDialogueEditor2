#pragma once

#include <DecToolsBox/abstract/singleton.h>

class EditorLockWindow : public Singleton<EditorLockWindow>{
public:
    void process();
private:
    bool m_opened = false;
};
