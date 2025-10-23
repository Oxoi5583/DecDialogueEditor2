#include "obj/abstract/clickable.h"
#include "server/mouse_server.h"
#include "server/object_server.h"


ClickableObject::ClickableObject(){
    BIND_CLASS(ClickableObject);
}
ClickableObject::~ClickableObject(){

}

void ClickableObject::ready(){

}
void ClickableObject::pre_process(){
    m_was_just_clicked = false;
    m_was_just_released = false;

    if(this->was_hovered()){
        if(MouseServer::Ref()->is_just_clicked()){
            m_was_just_clicked = true;
            m_was_clicked = true;
        }
    }

    if(MouseServer::Ref()->is_just_released()){
        m_was_just_released = true;
        m_was_clicked = false;
    }
}
void ClickableObject::process(){

}
void ClickableObject::post_process(){

}
void ClickableObject::draw(){

}

bool ClickableObject::was_just_clicked(){
    return m_was_just_clicked;
}
bool ClickableObject::was_clicked(){
    return m_was_clicked;
}
bool ClickableObject::was_just_released(){
    return m_was_just_released;
}
