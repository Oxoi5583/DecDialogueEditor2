#include "engine/texture_loader.h"

#include "DecToolsBox/debug/messenger.h"
#include "engine/renderer.h"
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace fs = std::filesystem;

void EngineTextureLoader::load(){
    if(m_is_first_load){
        if(!m_load_texture()){
            ERROR_MSG("Textures first loading failed.");
            exit(-1);
        }else{
            EngineRenderer::Ref()->set_texture_data(m_texture_array);
            SUCCESS_MSG("Textures first loaded successfully.");
        }
        m_is_first_load = false;
    }else{
        if(!m_load_texture()){
            ERROR_MSG("Textures loading failed. Setting will not be changed.");
        }else{
            EngineRenderer::Ref()->set_texture_data(m_texture_array);
            SUCCESS_MSG("Textures loaded successfully.");
        }
    }
}
void EngineTextureLoader::init(){
    load();
}

bool EngineTextureLoader::m_load_texture(){
    m_texture_array_layer.clear();

    int error_found = 0;

    if(!m_is_first_load){
        glDeleteTextures(1, &m_texture_array);
    }
    glGenTextures(1, &m_texture_array);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);

    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (!glTexStorage3D){
        std::cout << "GL_VENDOR:   "   << glGetString(GL_VENDOR)   << std::endl;
        std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
        ERROR_MSG("Current OpenGL Version : " << reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        ERROR_MSG("Please upgrade OpenGL to >4.2 version.");
        error_found++;
    }
    
    int mip_levels = static_cast<int>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mip_levels, GL_RGBA8, m_width, m_height, m_file_names.size());

    int layer = 0;
    for(auto& it : m_file_names){
        int file_id = it.file_id;
        std::string file_name = it.file_name;

        if(!fs::exists(file_name)){
            ERROR_MSG("Texture file not exists : " << file_name);
            error_found++;
            continue;
        }

        fs::path fs_file_name = fs::path(file_name);
        if(fs_file_name.extension().string() != ".png"){
            ERROR_MSG("Texture file is not .png : " << file_name);
            error_found++;
            continue;
        }
        
        int w, h, channels;
        unsigned char* data = stbi_load(file_name.c_str(), &w, &h, &channels, 4);
        
        bool is_image_loaded = true;
        if(data == nullptr){
            is_image_loaded = false;
            ERROR_MSG("Texture is not able to read : " << file_name);
            error_found++;
        }
        if(w != m_width || h != m_height){
            is_image_loaded = false;
            ERROR_MSG("Texture size is not " << m_width << "," << m_height << " : " << file_name);
            error_found++;
        }

        if(is_image_loaded){
            //glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, m_width, m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
            m_texture_array_layer.emplace(file_id, layer);

            layer++;
            SUCCESS_MSG("Texture loaded successfully : " << file_name);
        }else{
            ERROR_MSG("Texture loading failed : " << file_name);
            error_found++;
        }

        stbi_image_free(data);
    }


    if(error_found){
        return false;
    }else{
        return true;
    }
}

GLuint& EngineTextureLoader::get_data(){
    return m_texture_array;
}
int EngineTextureLoader::get_texture_layer(int p_file_id){
    if(!m_texture_array_layer.contains(p_file_id)){
        return -1;
    }
    return m_texture_array_layer[p_file_id];
}