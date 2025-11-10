#pragma once

#include <glm/glm.hpp>
#include <struct/shape/rect2.h>
#include <vector>
#include "DecToolsBox/abstract/singleton.h"
#include "engine/texture_loader.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float4.hpp"

using namespace glm;

typedef unsigned int Binary;
typedef unsigned int ShaderType;
typedef unsigned int TextureId;

class EngineRenderer : public Singleton<EngineRenderer>{
private:
    mat4 m_view_buffer;
    mat4 m_projection_buffer;

    Binary basic_vertex_shader;
    Binary basic_fragment_shader;
    Binary basic_shader_programme;

    GLuint texture_data;

    struct {
        const int max_render_size = 500000;
        const uint transform_location = 2;
        uint color_location = 6;
        uint texture_layer_location = 7;

        Binary VAO;
        Binary VBO;
        Binary EBO;

        GLuint view_loc;
        GLuint projection_loc;
        GLuint texture_data_loc;

        struct {
            Binary transform_vbo;
            Binary color_vbo;
            Binary texture_layer_vbo;
        } instance_data;

        struct {
            Binary vertex_shader;
            Binary fragment_shader;
            Binary programme;
        } shader;

        std::vector<mat4> transforms;
        std::vector<vec4> colors;
        std::vector<int> texture_layers; 

        float vertices[16] = {
            0.5f,  0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 1.0f
        };

        unsigned int indices[6] = {
            0, 1, 3,
            1, 2, 3
        };
    } m_rect_data;

    struct {
        const int max_render_size = 500000;
        const uint transform_location = 2;
        const uint color_location = 6;

        Binary VAO;
        Binary VBO;
        Binary EBO;

        GLuint view_loc;
        GLuint projection_loc;

        struct {
            Binary transform_vbo;
            Binary color_vbo;
        } instance_data;

        struct {
            Binary vertex_shader;
            Binary fragment_shader;
            Binary programme;
        } shader;
        
        std::vector<mat4> transforms;
        std::vector<float> radiuses;
        std::vector<vec4> colors;
        std::vector<int> texture_layers;

        float vertices[16] = {
            0.5f,  0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 1.0f
        };

        unsigned int indices[6] = {
            0, 1, 3,
            1, 2, 3
        };
    } m_circle_data;
    
    struct {
        const int max_render_size = 500000;
        const uint instance_width_location = 1;
        const uint instance_start_location = 2;
        const uint instance_end_location = 3;
        const uint color_location = 10;

        Binary VAO;
        Binary VBO;
        Binary EBO;
        
        GLuint view_loc;
        GLuint projection_loc;


        struct {
            Binary width_vbo;
            Binary start_vbo;
            Binary end_vbo;
            Binary color_vbo;
        } instance_data;

        struct {
            Binary vertex_shader;
            Binary fragment_shader;
            Binary programme;
        } shader;

        std::vector<vec2> fm_points;
        std::vector<vec2> to_points;
        std::vector<float> widths;
        std::vector<vec4> colors;
        float vertices[8] = {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
        };

        unsigned int indices[6] = {
            0, 1, 3,
            1, 2, 3
        };
    } m_line_data;

    template<typename  T>
    void m_quick_clear_list(std::vector<T>& p_list);

    void m_compile();
    void m_init_render_rect();
    void m_init_render_circle();
    void m_init_render_line();
    void m_init_uniform_loc();
    void m_render_rect();
    void m_render_circle();
    void m_render_line();
public:
    Binary craete_shader_binary(ShaderType p_type, const char* p_src);
    void init();
    void render();
    void destory_all();

    void set_texture_data(GLuint p_data);

    void clear_draw_list();
    void draw_rect(Rect2 p_rect, vec4 p_color, TextureId p_id);
    void draw_circle(vec2 p_pos, float p_radius, vec4 p_color, TextureId p_id);
    void draw_line(vec2 p_fm, vec2 p_to, vec4 p_color, float p_width);

    void refresh_buffer();
};