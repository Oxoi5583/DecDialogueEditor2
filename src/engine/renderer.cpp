#include "engine/renderer.h"
#include "DecToolsBox/debug/messenger.h"
#include "SDL3/SDL_timer.h"
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

layout (location = 6) in vec4 i_color;

out vec4 f_color;
out vec2 f_uv;

void main() {
    gl_Position = projection * view * transform * vec4(a_pos, 0.0, 1.0);

    f_color = i_color;
    f_uv = a_uv;
}
)";

const char* instanced_rect_fragment_shader_src = R"(
#version 330 core
in vec4 f_color;
in vec2 f_uv;

out vec4 FragColor;

void main() {
    FragColor = f_color;
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

mat2 rotate(float angle) {
    return mat2(cos(angle), -sin(angle),
                sin(angle),  cos(angle));
}

void main() {
    float half_width = width / 2.0;
    float dist = distance(instance_start, instance_end);
    vec2 dir = normalize(instance_end - instance_start);

    vec2 l_dir = rotate(radians(-90.0)) * dir;
    vec2 r_dir = rotate(radians(90.0)) * dir;

    vec2 left_down = instance_start + (l_dir * half_width);
    vec2 right_down = instance_start + (r_dir * half_width);
    vec2 left_up = left_down + (dir * dist);
    vec2 right_up = right_down + (dir * dist);

    vec2 real_pos = mix(
        mix(left_down, right_down, a_pos.x),
        mix(left_up, right_up, a_pos.x),
        a_pos.y
    );

    gl_Position = projection * view * vec4(real_pos, 0.0, 1.0);

    f_color = i_color;
}
)";
const char* instanced_line_fragment_shader_src = R"(
#version 330 core
in vec4 f_color;

out vec4 FragColor;

void main() {
    FragColor = f_color;
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

void EngineRenderer::compile(){
    basic_vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, basic_vertex_shader_src);
    basic_fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, basic_fragment_shader_src);

    basic_shader_programme = glCreateProgram();
    glAttachShader(basic_shader_programme, basic_vertex_shader);
    glAttachShader(basic_shader_programme, basic_fragment_shader);
    glLinkProgram(basic_shader_programme);

    glDeleteShader(basic_vertex_shader);
    glDeleteShader(basic_fragment_shader);
    
    instanced_rect_vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, instanced_rect_vertex_shader_src);
    instanced_rect_fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, instanced_rect_fragment_shader_src);

    instanced_rect_shader_programme = glCreateProgram();
    glAttachShader(instanced_rect_shader_programme, instanced_rect_vertex_shader);
    glAttachShader(instanced_rect_shader_programme, instanced_rect_fragment_shader);
    glLinkProgram(instanced_rect_shader_programme);

    glDeleteShader(instanced_rect_vertex_shader);
    glDeleteShader(instanced_rect_fragment_shader);
    
    instanced_circle_vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, instanced_circle_vertex_shader_src);
    instanced_circle_fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, instanced_circle_fragment_shader_src);

    instanced_circle_shader_programme = glCreateProgram();
    glAttachShader(instanced_circle_shader_programme, instanced_circle_vertex_shader);
    glAttachShader(instanced_circle_shader_programme, instanced_circle_fragment_shader);
    glLinkProgram(instanced_circle_shader_programme);

    glDeleteShader(instanced_circle_vertex_shader);
    glDeleteShader(instanced_circle_fragment_shader);
    
    instanced_line_vertex_shader = craete_shader_binary(GL_VERTEX_SHADER, instanced_line_vertex_shader_src);
    instanced_line_fragment_shader = craete_shader_binary(GL_FRAGMENT_SHADER, instanced_line_fragment_shader_src);

    instanced_line_shader_programme = glCreateProgram();
    glAttachShader(instanced_line_shader_programme, instanced_line_vertex_shader);
    glAttachShader(instanced_line_shader_programme, instanced_line_fragment_shader);
    glLinkProgram(instanced_line_shader_programme);

    glDeleteShader(instanced_line_vertex_shader);
    glDeleteShader(instanced_line_fragment_shader);
}

void EngineRenderer::m_render_rect(){
    Binary VAO;
    Binary VBO;
    Binary EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_draw_rect_instanced_buffer.vertices), m_draw_rect_instanced_buffer.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_draw_rect_instanced_buffer.indices), m_draw_rect_instanced_buffer.indices, GL_STATIC_DRAW);

    glUseProgram(instanced_rect_shader_programme);

    uint transform_location = 2;
    GLuint instance_transform_VBO;
    glGenBuffers(1, &instance_transform_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_transform_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_rect_instanced_buffer.transforms.size() * sizeof(mat4), m_draw_rect_instanced_buffer.transforms.data(), GL_STATIC_DRAW);
    std::size_t vec4_size = sizeof(vec4);
    for (int i = 0; i < 4; ++i) {
        glVertexAttribPointer(transform_location + i, 4, GL_FLOAT, GL_FALSE,
            sizeof(mat4), (void*)(i * vec4_size));
        glEnableVertexAttribArray(transform_location + i);
        glVertexAttribDivisor(transform_location + i, 1);
    }
    
    uint color_location = 6;
    GLuint instance_color_VBO;
    glGenBuffers(1, &instance_color_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_rect_instanced_buffer.colors.size() * sizeof(vec4), m_draw_rect_instanced_buffer.colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(color_location);
    glVertexAttribDivisor(color_location, 1);

    GLuint view_loc = glGetUniformLocation(instanced_rect_shader_programme, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view_buffer));
    GLuint projection_loc = glGetUniformLocation(instanced_rect_shader_programme, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection_buffer));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_draw_rect_instanced_buffer.transforms.size()));
}
void EngineRenderer::m_render_circle(){
    Binary VAO;
    Binary VBO;
    Binary EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_draw_circle_instanced_buffer.vertices), m_draw_circle_instanced_buffer.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_draw_circle_instanced_buffer.indices), m_draw_circle_instanced_buffer.indices, GL_STATIC_DRAW);

    glUseProgram(instanced_circle_shader_programme);

    uint transform_location = 2;
    GLuint instance_transform_VBO;
    glGenBuffers(1, &instance_transform_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_transform_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_circle_instanced_buffer.transforms.size() * sizeof(mat4), m_draw_circle_instanced_buffer.transforms.data(), GL_STATIC_DRAW);
    std::size_t vec4_size = sizeof(vec4);
    for (int i = 0; i < 4; ++i) {
        glVertexAttribPointer(transform_location + i, 4, GL_FLOAT, GL_FALSE,
            sizeof(mat4), (void*)(i * vec4_size));
        glEnableVertexAttribArray(transform_location + i);
        glVertexAttribDivisor(transform_location + i, 1);
    }
    
    uint color_location = 6;
    GLuint instance_color_VBO;
    glGenBuffers(1, &instance_color_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_circle_instanced_buffer.colors.size() * sizeof(vec4), m_draw_circle_instanced_buffer.colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(color_location);
    glVertexAttribDivisor(color_location, 1);
    
    GLuint view_loc = glGetUniformLocation(instanced_circle_shader_programme, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view_buffer));
    GLuint projection_loc = glGetUniformLocation(instanced_circle_shader_programme, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection_buffer));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_draw_circle_instanced_buffer.transforms.size()));
}
void EngineRenderer::m_render_line(){
    DEBUG_MSG("line count : " << m_draw_line_instanced_buffer.colors.size());
    Binary VAO;
    Binary VBO;
    Binary EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_draw_line_instanced_buffer.vertices), m_draw_line_instanced_buffer.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_draw_line_instanced_buffer.indices), m_draw_line_instanced_buffer.indices, GL_STATIC_DRAW);
    
    glUseProgram(instanced_line_shader_programme);

    uint instance_width_location = 1;
    GLuint instance_width_VBO;
    glGenBuffers(1, &instance_width_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_width_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_line_instanced_buffer.widths.size() * sizeof(float), m_draw_line_instanced_buffer.widths.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(instance_width_location, 1, GL_FLOAT, GL_FALSE,sizeof(float), (void*)(0));
    glEnableVertexAttribArray(instance_width_location);
    glVertexAttribDivisor(instance_width_location, 1);

    uint instance_start_location = 2;
    GLuint instance_start_VBO;
    glGenBuffers(1, &instance_start_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_start_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_line_instanced_buffer.fm_points.size() * sizeof(vec2), m_draw_line_instanced_buffer.fm_points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(instance_start_location, 2, GL_FLOAT, GL_FALSE,2 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(instance_start_location);
    glVertexAttribDivisor(instance_start_location, 1);
    
    uint instance_end_location = 3;
    GLuint instance_end_VBO;
    glGenBuffers(1, &instance_end_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_end_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_line_instanced_buffer.to_points.size() * sizeof(vec2), m_draw_line_instanced_buffer.to_points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(instance_end_location, 2, GL_FLOAT, GL_FALSE,2 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(instance_end_location);
    glVertexAttribDivisor(instance_end_location, 1);

    uint color_location = 10;
    GLuint instance_color_VBO;
    glGenBuffers(1, &instance_color_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, instance_color_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_draw_line_instanced_buffer.colors.size() * sizeof(vec4), m_draw_line_instanced_buffer.colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(color_location, 4, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(color_location);
    glVertexAttribDivisor(color_location, 1);
    
    GLuint view_loc = glGetUniformLocation(instanced_line_shader_programme, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(m_view_buffer));
    GLuint projection_loc = glGetUniformLocation(instanced_line_shader_programme, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(m_projection_buffer));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_draw_line_instanced_buffer.fm_points.size()));
}

void EngineRenderer::render(){
    m_view_buffer = GraphCamera::Ref()->get_view();
    m_projection_buffer = GraphCamera::Ref()->get_projection();

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
    glDeleteProgram(instanced_rect_shader_programme);
    glDeleteProgram(instanced_circle_shader_programme);
}

template<typename  T>
void EngineRenderer::m_quick_clear_list(std::vector<T>& p_list){
    std::vector<T>().swap(p_list);
}

void EngineRenderer::clear_draw_list(){
    m_quick_clear_list(m_draw_rect_instanced_buffer.transforms);
    m_quick_clear_list(m_draw_rect_instanced_buffer.colors);
    m_quick_clear_list(m_draw_rect_instanced_buffer.texture_ids);
    
    m_quick_clear_list(m_draw_circle_instanced_buffer.transforms);
    m_quick_clear_list(m_draw_circle_instanced_buffer.radiuses);
    m_quick_clear_list(m_draw_circle_instanced_buffer.colors);
    m_quick_clear_list(m_draw_circle_instanced_buffer.texture_ids);
    
    m_quick_clear_list(m_draw_line_instanced_buffer.fm_points);
    m_quick_clear_list(m_draw_line_instanced_buffer.to_points);
    m_quick_clear_list(m_draw_line_instanced_buffer.widths);
    m_quick_clear_list(m_draw_line_instanced_buffer.colors);
}


void EngineRenderer::draw_rect(Rect2 p_rect, vec4 p_color, TextureId p_id){
    m_draw_rect_instanced_buffer.texture_ids.push_back(p_id);
    m_draw_rect_instanced_buffer.colors.push_back(p_color);
    mat4 transform = mat4(1.0);
    transform = glm::translate(transform, vec3(p_rect.get_position(),0.0f));
    transform = glm::scale(transform, vec3(p_rect.get_size(),0.0f));
    m_draw_rect_instanced_buffer.transforms.push_back(transform);
}
void EngineRenderer::draw_circle(vec2 p_pos, float p_radius, vec4 p_color, TextureId p_id){
    m_draw_circle_instanced_buffer.texture_ids.push_back(p_id);
    m_draw_circle_instanced_buffer.colors.push_back(p_color);
    mat4 transform = mat4(1.0);
    transform = glm::translate(transform, vec3(p_pos,0.0f));
    transform = glm::scale(transform, vec3(p_radius * 2, p_radius * 2,0.0f));
    m_draw_circle_instanced_buffer.transforms.push_back(transform);
    m_draw_circle_instanced_buffer.radiuses.push_back(p_radius);
}
void EngineRenderer::draw_line(vec2 p_fm, vec2 p_to, vec4 p_color, float p_width){
    m_draw_line_instanced_buffer.fm_points.push_back(p_fm);
    m_draw_line_instanced_buffer.to_points.push_back(p_to);
    m_draw_line_instanced_buffer.colors.push_back(p_color);
    m_draw_line_instanced_buffer.widths.push_back(p_width);
}