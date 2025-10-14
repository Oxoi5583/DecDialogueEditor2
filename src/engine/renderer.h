#pragma once

#include <glm/glm.hpp>
#include <struct/rect2.h>
#include <vector>
#include "DecToolsBox/abstract/singleton.h"
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

    Binary instanced_rect_vertex_shader;
    Binary instanced_rect_fragment_shader;
    Binary instanced_rect_shader_programme;

    Binary instanced_circle_vertex_shader;
    Binary instanced_circle_fragment_shader;
    Binary instanced_circle_shader_programme;

    Binary instanced_line_vertex_shader;
    Binary instanced_line_fragment_shader;
    Binary instanced_line_shader_programme;
    struct {
        std::vector<mat4> transforms;
        std::vector<vec4> colors;
        std::vector<unsigned int> texture_ids;

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
    } m_draw_rect_instanced_buffer;

    struct {
        std::vector<mat4> transforms;
        std::vector<float> radiuses;
        std::vector<vec4> colors;
        std::vector<unsigned int> texture_ids;

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
    } m_draw_circle_instanced_buffer;
    
    struct {
        std::vector<mat4> fm_point_transforms;
        std::vector<vec2> fm_point;
        std::vector<vec2> to_point;
        std::vector<vec4> colors;
        float vertices[16] = {
            0.0f,  0.0f,
            0.0f, 1.0f,
        };
    } m_draw_line_instanced_buffer;

    template<typename  T>
    void m_quick_clear_list(std::vector<T>& p_list);

    void m_render_rect();
    void m_render_circle();
    void m_render_line();
public:
    Binary craete_shader_binary(ShaderType p_type, const char* p_src);
    void compile();
    void render();
    void destory_all();

    void clear_draw_list();
    void draw_rect(Rect2 p_rect, vec4 p_color, TextureId p_id);
    void draw_circle(vec2 p_pos, float p_radius, vec4 p_color, TextureId p_id);
    void draw_line(vec2 p_fm, vec2 p_to, vec4 p_color);
};