#pragma once

#include "editor/space.h"
#include "obj/abstract/clickable.h"

class EditorComponentBase : public ClickableObject{
private:

public:
    EditorComponentBase(){};
    ~EditorComponentBase(){};
    
    virtual void ui_init(EditorSpace* p_space){};
    virtual void ui_update(){};
    virtual void ui_draw(){};
};