#include "system/character/manager.h"

CharacterManager::CharacterManager(){}
CharacterManager::~CharacterManager(){}

std::map<CharacterID, Character> CharacterManager::get_data(){
    return m_data;
}

void CharacterManager::create(){
    CharacterID new_cid = m_generate_cid();
    m_data.emplace(new_cid, Character(new_cid));
}