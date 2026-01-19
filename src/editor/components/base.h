#pragma once

#include "editor/space.h"
#include "system/obj/abstract/clickable.h"

class EditorComponentBase : public ClickableObject{
private:

public:
    EditorComponentBase();
    ~EditorComponentBase();
    
    virtual void ui_init(EditorSpace* p_space){};
    virtual void ui_update(){};
    virtual void ui_draw(){};
    
    void ready();
    void pre_process();
    void process();
    void post_process();
    void draw();
};