#pragma once

#include <string>

typedef std::string CharacterID;

class Character {
private:
    CharacterID m_id; 
public:
    Character(CharacterID p_id)
    : m_id(p_id) {}
    ~Character() = default;
};