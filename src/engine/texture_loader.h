#pragma once

#include "glad/glad.h"
#include "DecToolsBox/abstract/singleton.h"
#include <map>

class EngineTextureLoader : public Singleton<EngineTextureLoader>{
private:
    GLuint m_texture_array;

    int m_width = 500;
    int m_height = 500;

    struct TextureFile{
        int file_id;
        std::string file_name;
    };
    const std::vector<TextureFile> m_file_names = {
        {0 ,"assets/test_texture.png"},
    };

    std::map<int, int> m_texture_array_layer;

    bool m_load_texture();

    bool m_is_first_load = true;
public:
    void load();
    void init();

    GLuint& get_data();
    int get_texture_layer(int p_file_id);
};