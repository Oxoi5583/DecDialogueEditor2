#include "editor/space_base.h"


LID EditorSpaceBase::m_next_id = 100;
LID EditorSpaceBase::fetch_next_id(){
    LID return_id = m_next_id;
    m_next_id++;
    return return_id;
}