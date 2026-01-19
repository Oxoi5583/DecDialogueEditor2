#pragma once
#include "system/character/character.h"
#include "DecToolsBox/abstract./singleton.h"
#include "DecToolsBox/struct/random_code.h"
#include <map>

class CharacterManager : public Singleton<CharacterManager> {
private:
    CharacterID m_generate_cid(){
        const int id_length = 16;
        return RandomCode(16).get();
    }

    std::map<CharacterID, Character> m_data;
public:
    CharacterManager();
    ~CharacterManager();

    std::map<CharacterID, Character> get_data();
    void create();
};