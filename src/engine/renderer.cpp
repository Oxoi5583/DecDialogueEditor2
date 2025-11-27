#include "engine/renderer.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_timer.h"
#include "engine/texture_loader.h"
#include "ext/debug/messenger_ext.h"
#include "engine/window.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "graph/camera.h"


const char* basic_vertex_shader_src = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 transform;
void main() {
    gl_Position = transform * vec4(aPos, 1.0);
}
)";

const char* basic_fragment_shader_src = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

const char* instanced_rect_vertex_shader_src = R"(
#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in mat4 transform;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2DArray texture_array;

layout (location = 6) in vec4 i_color;
layout (location = 7) in int i_texture_layer;

out vec4 f_color;
out vec2 f_uv;
flat out int f_texture_layer;

void main() {
    gl_Position = projection * view * transform * vec4(a_pos, 0.0, 1.0);

    f_color = i_color;
    f_uv = a_uv;
    f_texture_layer = i_texture_layer;
}
)";

const char* instanced_rect_fragment_shader_src = R"(
#version 330 core
in vec4 f_color;
in vec2 f_uv;
flat in int f_texture_layer;

out vec4 FragColor;

uniform sampler2DArray texture_array;

void main() {
    if(f_texture_layer == -1){
        FragColor = f_color;
    }else{
        FragColor = texture(texture_array, vec3(f_uv, f_texture_layer));
    }
}
)";

const char* instanced_circle_vertex_shader_src = R"(
#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in mat4 transform;

uniform mat4 view;
uniform mat4 projection;

layout (location = 6) in vec4 i_color;

out vec4 f_color;
out vec2 f_uv;

void main() {
    gl_Position = projection * view * transform * vec4(a_pos, 0.0, 1.0);

    f_color = i_color;
    f_uv = a_uv;
}
)";

const char* instanced_circle_fragment_shader_src = R"(
#version 330 core
in vec4 f_color;
in vec2 f_uv;

out vec4 FragColor;

void main() {
    vec2 center = vec2(0.5,0.5);
    float dist = distance(center, f_uv);
    float alpha = 1.0 - step(0.5, dist);

    FragColor = vec4(f_color.rgb,f_color.a * alpha);
}
)";
const char* instanced_line_vertex_shader_src = R"(
#version 330 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in float width;
layout (location = 2) in vec2 instance_start;
layout (location = 3) in vec2 instance_end;

uniform mat4 view;
uniform mat4 projection;

layout (location = 10) in vec4 i_color;

out vec4 f_color;
out float f_width;
out float f_dist;

mat2 rotate(float angle) {
    return mat2(cos(angle), -sin(angle),
                sin(angle),  cos(angle));
}

void main() {
    float half_width = width / 2.0;
    vec2 dir = normalize(instance_end - instance_start);

    vec2 start = instance_start - (dir * half_width);
    vec2 end = instance_end + (dir * half_width);

    float dist = distance(start, end);

    vec2 l_dir = rotate(radians(-90.0)) * dir;
    vec2 r_dir = rotate(radians(90.0)) * dir;

    vec2 left_down = start + (l_dir * half_width);
    vec2 right_down = start + (r_dir * half_width);
    vec2 left_up = left_down + (dir * dist);
    vec2 right_up = right_down + (dir * dist);

    vec2 real_pos = mix(
        mix(left_down, right_down, a_pos.x),
        mix(left_up, right_up, a_pos.x),
        a_pos.y
    );

    gl_Position = projection * view * vec4(real_pos, 0.0, 1.0);

    f_color = i_color;
    f_width = width;
    f_dist = (a_pos.x - 0.5f) * width;
}
)";
const char* instanced_line_fragment_shader_src = R"(
#version 330 core
in vec4 f_color;
in float f_width;
in float f_dist;

out vec4 FragColor;

uniform float feather = 1.0;

void main() {
    float d = abs(f_dist);
    float alpha = smoothstep(f_width / 2.0f, (f_width / 2.0f) - feather, d);

    FragColor = vec4(f_color.rgb, f_color.a * alpha);
}
)";


Binary EngineRenderer::craete_shader_binary(ShaderType p_type, const char* p_src){
    Binary shader = glCreateShader(p_type);

    glShaderSource(shader, 1, &p_src, nullptr);
    glCompileShader(shader);

    int is_success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_success);
    if(!is_success){
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        ERROR_MSG("Creating shader binary failed : " << info_log);
    }

    return shader;
}

void EngineRenderer::m_compile(){
    basic_vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, basic_vertex_shader_src);
    basic_fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, basic_fragment_shader_src);

    basic_shader_programme = glCreateProgram();
    glAttachShader(basic_shader_programme, basic_vertex_shader);
    glAttachShader(basic_shader_programme, basic_fragment_shader);
    glLinkProgram(basic_shader_programme);

    glDeleteShader(basic_vertex_shader);
    glDeleteShader(basic_fragment_shader);
    
    m_rect_data.shader.vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, instanced_rect_vertex_shader_src);
    m_rect_data.shader.fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, instanced_rect_fragment_shader_src);

    m_rect_data.shader.programme = glCreateProgram();
    glAttachShader(m_rect_data.shader.programme, m_rect_data.shader.vertex_shader);
    glAttachShader(m_rect_data.shader.programme, m_rect_data.shader.fragment_shader);
    glLinkProgram(m_rect_data.shader.programme);

    glDeleteShader(m_rect_data.shader.vertex_shader);
    glDeleteShader(m_rect_data.shader.fragment_shader);
    
    m_circle_data.shader.vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, instanced_circle_vertex_shader_src);
    m_circle_data.shader.fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, instanced_circle_fragment_shader_src);

    m_circle_data.shader.programme = glCreateProgram();
    glAttachShader(m_circle_data.shader.programme, m_circle_data.shader.vertex_shader);
    glAttachShader(m_circle_data.shader.programme, m_circle_data.shader.fragment_shader);
    glLinkProgram(m_circle_data.shader.programme);

    glDeleteShader(m_circle_data.shader.vertex_shader);
    glDeleteShader(m_circle_data.shader.fragment_shader);
    
    m_line_data.shader.vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, instanced_line_vertex_shader_src);
    m_line_data.shader.fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, instanced_line_fragment_shader_src);

    m_line_data.shader.programme = glCreateProgram();
    glAttachShader(m_line_data.shader.programme, m_line_data.shader.vertex_shader);
    glAttachShader(m_line_data.shader.programme, m_line_data.shader.fragment_shader);
    glLinkProgram(m_line_data.shader.programme);

    glDeleteShader(m_line_data.shader.vertex_shader);
    glDeleteShader(m_line_data.shader.fragment_shader);
}
void EngineRenderer::init(){
    this->m_compile();
    this->m_init_render_rect();
    this->m_init_render_circle();
    this->m_init_render_line();
    this->m_init_uniform_loc();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void EngineRenderer::m_init_uniform_loc(){
    m_rect_data.view_loc = glGetUniformLocation(m_rect_data.shader.programme, "view");
    m_rect_data.projection_loc = glGetUniformLocation(m_rect_data.shader.programme, "projection");
    m_rect_data.texture_data_loc = glGetUniformLocation(m_rect_data.shader.programme, "texture_array");

    m_circle_data.view_loc = glGetUniformLocation(m_circle_data.shader.programme, "view");
    m_circle_data.projection_loc = glGetUniformLocation(m_circle_data.shader.programme, "projection");

    m_line_data.view_loc = glGetUniformLocation(m_line_data.shader.programme, "view");
    m_line_data.projection_loc = glGetUniformLocation(m_line_data.shader.programme, "projection");
}
void EngineRenderer::m_init_render_rect(){
    glGenVertexArrays(1, &m_rect_data.VAO);
    glGenBuffers(1, &m_rect_data.VBO);
    glGenBuffers(1, &m_rect_data.EBO);

    glBindVertexArray(m_rect_data.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_rect_data.vertices), m_rect_data.vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rect_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_rect_data.indices), m_rect_data.indices, GL_DYNAMIC_DRAW);


    glGenBuffers(1, &m_rect_data.instance_data.transform_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.instance_data.transform_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_rect_data.max_render_size * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
    std::size_t vec4_size = sizeof(vec4);
    for (int i = 0; i < 4; ++i) {
        glVertexAttribPointer(m_rect_data.transform_location + i, 4, GL_FLOAT, GL_FALSE,
            sizeof(mat4), (void*)(i * vec4_size));
        glEnableVertexAttribArray(m_rect_data.transform_location + i);
        glVertexAttribDivisor(m_rect_data.transform_location + i, 1);
    }


    glGenBuffers(1, &m_rect_data.instance_data.color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.instance_data.color_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_rect_data.max_render_size * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_rect_data.color_location, 4, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(m_rect_data.color_location);
    glVertexAttribDivisor(m_rect_data.color_location, 1);


    glGenBuffers(1, &m_rect_data.instance_data.texture_layer_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.instance_data.texture_layer_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_rect_data.max_render_size * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_rect_data.texture_layer_location, 1, GL_FLOAT, GL_FALSE, sizeof(int), (void*)(0));
    glEnableVertexAttribArray(m_rect_data.texture_layer_location);
    glVertexAttribDivisor(m_rect_data.texture_layer_location, 1);
}
void EngineRenderer::m_render_rect(){
    glBindVertexArray(m_rect_data.VAO);
    glUseProgram(m_rect_data.shader.programme);

    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.instance_data.transform_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_rect_data.transforms.size() * sizeof(mat4), m_rect_data.transforms.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.instance_data.color_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_rect_data.colors.size() * sizeof(vec4), m_rect_data.colors.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_rect_data.instance_data.texture_layer_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_rect_data.texture_layers.size() * sizeof(int), m_rect_data.texture_layers.data());

    glUniformMatrix4fv(m_rect_data.view_loc, 1, GL_FALSE, glm::value_ptr(m_view_buffer));
    glUniformMatrix4fv(m_rect_data.projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection_buffer));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_data);
    glUniform1i(m_rect_data.texture_data_loc, 0);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_rect_data.transforms.size()));
}
void EngineRenderer::m_init_render_circle(){
    glGenVertexArrays(1, &m_circle_data.VAO);
    glGenBuffers(1, &m_circle_data.VBO);
    glGenBuffers(1, &m_circle_data.EBO);
    
    glBindVertexArray(m_circle_data.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_circle_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_circle_data.vertices), m_circle_data.vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_circle_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_circle_data.indices), m_circle_data.indices, GL_DYNAMIC_DRAW);


    glGenBuffers(1, &m_circle_data.instance_data.transform_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_circle_data.instance_data.transform_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_circle_data.max_render_size * sizeof(mat4), nullptr, GL_DYNAMIC_DRAW);
    std::size_t vec4_size = sizeof(vec4);
    for (int i = 0; i < 4; ++i) {
        glVertexAttribPointer(m_circle_data.transform_location + i, 4, GL_FLOAT, GL_FALSE,
            sizeof(mat4), (void*)(i * vec4_size));
        glEnableVertexAttribArray(m_circle_data.transform_location + i);
        glVertexAttribDivisor(m_circle_data.transform_location + i, 1);
    }

    glGenBuffers(1, &m_circle_data.instance_data.color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_circle_data.instance_data.color_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_circle_data.max_render_size * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_circle_data.color_location, 4, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(m_circle_data.color_location);
    glVertexAttribDivisor(m_circle_data.color_location, 1);
}
void EngineRenderer::m_render_circle(){
    glBindVertexArray(m_circle_data.VAO);
    glUseProgram(m_circle_data.shader.programme);

    glBindBuffer(GL_ARRAY_BUFFER, m_circle_data.instance_data.transform_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_circle_data.transforms.size() * sizeof(mat4), m_circle_data.transforms.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, m_circle_data.instance_data.color_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_circle_data.colors.size() * sizeof(vec4), m_circle_data.colors.data());

    glUniformMatrix4fv(m_circle_data.view_loc, 1, GL_FALSE, glm::value_ptr(m_view_buffer));
    glUniformMatrix4fv(m_circle_data.projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection_buffer));

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_circle_data.transforms.size()));
}
void EngineRenderer::m_init_render_line(){
    glGenVertexArrays(1, &m_line_data.VAO);
    glGenBuffers(1, &m_line_data.VBO);
    glGenBuffers(1, &m_line_data.EBO);

    glBindVertexArray(m_line_data.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_line_data.vertices), m_line_data.vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_line_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_line_data.indices), m_line_data.indices, GL_DYNAMIC_DRAW);
    

    glGenBuffers(1, &m_line_data.instance_data.width_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.width_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_line_data.max_render_size * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_line_data.instance_width_location, 1, GL_FLOAT, GL_FALSE,sizeof(float), (void*)(0));
    glEnableVertexAttribArray(m_line_data.instance_width_location);
    glVertexAttribDivisor(m_line_data.instance_width_location, 1);

    glGenBuffers(1, &m_line_data.instance_data.start_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.start_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_line_data.max_render_size * sizeof(vec2), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_line_data.instance_start_location, 2, GL_FLOAT, GL_FALSE,2 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(m_line_data.instance_start_location);
    glVertexAttribDivisor(m_line_data.instance_start_location, 1);

    glGenBuffers(1, &m_line_data.instance_data.end_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.end_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_line_data.max_render_size * sizeof(vec2), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_line_data.instance_end_location, 2, GL_FLOAT, GL_FALSE,2 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(m_line_data.instance_end_location);
    glVertexAttribDivisor(m_line_data.instance_end_location, 1);

    glGenBuffers(1, &m_line_data.instance_data.color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.color_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_line_data.max_render_size * sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(m_line_data.color_location, 4, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(m_line_data.color_location);
    glVertexAttribDivisor(m_line_data.color_location, 1);
}
void EngineRenderer::m_render_line(){
    glBindVertexArray(m_line_data.VAO);
    glUseProgram(m_line_data.shader.programme);

    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.width_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_line_data.widths.size() * sizeof(float), m_line_data.widths.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.start_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_line_data.fm_points.size() * sizeof(vec2), m_line_data.fm_points.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.end_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_line_data.to_points.size() * sizeof(vec2), m_line_data.to_points.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_line_data.instance_data.color_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_line_data.colors.size() * sizeof(vec4), m_line_data.colors.data());

    glUniformMatrix4fv(m_line_data.view_loc, 1, GL_FALSE, glm::value_ptr(m_view_buffer));
    glUniformMatrix4fv(m_line_data.projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection_buffer));

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_line_data.fm_points.size()));
}

void EngineRenderer::refresh_buffer(){
    m_view_buffer = GraphCamera::Ref()->get_view();
    m_projection_buffer = GraphCamera::Ref()->get_projection();
}

void EngineRenderer::render(){
    refresh_buffer();

    m_render_line();
    m_render_rect();
    m_render_circle();

    /*
    float vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
       -0.5f,  0.5f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    Binary VAO;
    Binary VBO;
    Binary EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    
    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(basic_shader_programme);

    mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, vec3(1.0,1.0,0.0));
    //transform = glm::rotate(transform, (float)SDL_GetTicks() / 1000.0f, vec3(0,0,1));
    int transform_loc = glGetUniformLocation(basic_shader_programme, "transform");
    glUniformMatrix4fv(transform_loc, 1,GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    */
}

void EngineRenderer::destory_all(){
    glDeleteProgram(basic_shader_programme);
    glDeleteProgram(m_rect_data.shader.programme);
    glDeleteProgram(m_circle_data.shader.programme);

    // rect
    glDeleteVertexArrays(1, &m_rect_data.VAO);
    glDeleteBuffers(1, &m_rect_data.VBO);
    glDeleteBuffers(1, &m_rect_data.EBO);
    glDeleteBuffers(1, &m_rect_data.instance_data.transform_vbo);
    glDeleteBuffers(1, &m_rect_data.instance_data.color_vbo);

    // circle
    glDeleteVertexArrays(1, &m_circle_data.VAO);
    glDeleteBuffers(1, &m_circle_data.VBO);
    glDeleteBuffers(1, &m_circle_data.EBO);
    glDeleteBuffers(1, &m_circle_data.instance_data.transform_vbo);
    glDeleteBuffers(1, &m_circle_data.instance_data.color_vbo);

    // line
    glDeleteVertexArrays(1, &m_line_data.VAO);
    glDeleteBuffers(1, &m_line_data.VBO);
    glDeleteBuffers(1, &m_line_data.EBO);
    glDeleteBuffers(1, &m_line_data.instance_data.width_vbo);
    glDeleteBuffers(1, &m_line_data.instance_data.start_vbo);
    glDeleteBuffers(1, &m_line_data.instance_data.end_vbo);
    glDeleteBuffers(1, &m_line_data.instance_data.color_vbo);

}

template<typename  T>
void EngineRenderer::m_quick_clear_list(std::vector<T>& p_list){
    std::vector<T>().swap(p_list);
}

void EngineRenderer::clear_draw_list(){
    m_quick_clear_list(m_rect_data.transforms);
    m_quick_clear_list(m_rect_data.colors);
    m_quick_clear_list(m_rect_data.texture_layers);
    
    m_quick_clear_list(m_circle_data.transforms);
    m_quick_clear_list(m_circle_data.radiuses);
    m_quick_clear_list(m_circle_data.colors);
    m_quick_clear_list(m_circle_data.texture_layers);
    
    m_quick_clear_list(m_line_data.fm_points);
    m_quick_clear_list(m_line_data.to_points);
    m_quick_clear_list(m_line_data.widths);
    m_quick_clear_list(m_line_data.colors);
}


void EngineRenderer::draw_rect(Rect2 p_rect, vec4 p_color, TextureId p_id){
    if(m_rect_data.colors.size() < m_rect_data.max_render_size){
        m_rect_data.texture_layers.push_back(EngineTextureLoader::Ref()->get_texture_layer(p_id));
        m_rect_data.colors.push_back(p_color);
        mat4 transform = mat4(1.0);
        transform = glm::translate(transform, vec3(p_rect.get_position(),0.0f));
        transform = glm::scale(transform, vec3(p_rect.get_size(),0.0f));
        m_rect_data.transforms.push_back(transform);
    }
}
void EngineRenderer::draw_circle(vec2 p_pos, float p_radius, vec4 p_color, TextureId p_id){
    if(m_circle_data.colors.size() < m_circle_data.max_render_size){
        m_circle_data.texture_layers.push_back(EngineTextureLoader::Ref()->get_texture_layer(p_id));
        m_circle_data.colors.push_back(p_color);
        mat4 transform = mat4(1.0);
        transform = glm::translate(transform, vec3(p_pos,0.0f));
        transform = glm::scale(transform, vec3(p_radius * 2, p_radius * 2,0.0f));
        m_circle_data.transforms.push_back(transform);
        m_circle_data.radiuses.push_back(p_radius);
    }
}
void EngineRenderer::draw_line(vec2 p_fm, vec2 p_to, vec4 p_color, float p_width){
    if(m_line_data.colors.size() < m_line_data.max_render_size){
        m_line_data.fm_points.push_back(p_fm);
        m_line_data.to_points.push_back(p_to);
        m_line_data.colors.push_back(p_color);
        m_line_data.widths.push_back(p_width);
    }
}
void EngineRenderer::set_texture_data(GLuint p_data){
    texture_data = p_data;
}
