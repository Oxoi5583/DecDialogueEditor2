#pragma once


#include "editor/layout.h"
#include "struct/shape/rect2.h"
#include <vector>

class EditorSpaceBase{
public:
    enum class Type{
        VERTICAL,
        HORIZONTAL
    };

    EditorSpaceBase(){}
    ~EditorSpaceBase();

    Type get_type();
    void set_type(Type p_type);
    double get_size();
    void set_size(double p_type);
protected:
    void get_parent(EditorSpaceBase* p_space);
    void set_parent(EditorSpaceBase* p_space);
    void append_child(EditorSpaceBase* p_space);
private:
    static LID m_next_id;
    static LID fetch_next_id();
    const LID m_id = fetch_next_id();

    EditorSpaceBase* m_parent_space = nullptr;
    std::vector<EditorSpaceBase*> m_children_space;

    double m_size; 
    Type m_type;
};