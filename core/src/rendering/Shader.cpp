#include "Shader.h"

#include <fstream>

static std::string read_shader(const std::string &filename) {
    std::ifstream file(filename);
    std::stringstream source;

    if (file.is_open()) {
        source << file.rdbuf();
    } else {
        std::cerr << "Couldn't open shader " << filename << std::endl;
        std::abort();
    }

    return source.str();
}

static unsigned int check_shader(unsigned int shader, GLenum type) {
    int success;
    char info_log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        switch (type) {
            case GL_VERTEX_SHADER: {
                std::cerr << "ERROR::VERTEX::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
                break;
            } 
            case GL_FRAGMENT_SHADER: {
                std::cerr << "ERROR::FRAGMENT::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
                break;
            }
        }
        return 0;
    }
    return shader;
}

static unsigned int check_shader_program(unsigned int id) {
    int success;
    char info_log[512];

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, info_log);
        std::cout << "ERROR::PROGRAM_LINKING_FAILED\n" << info_log << std::endl;
        return 0;
    }

    return id;
}

static unsigned int compile_shader(const std::string &source, GLenum type) {
    unsigned int shader;
    shader = glCreateShader(type);

    const char *c_source = source.c_str();
    glShaderSource(shader, 1, &c_source, NULL);
    glCompileShader(shader);
    return check_shader(shader, type);
}

static unsigned int create_shader_program(unsigned int vertex_shader, unsigned int fragment_shader) {
    unsigned int id = glCreateProgram();

    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    return check_shader_program(id);
}

Shader::Shader(const std::string &vertex_path, const std::string &fragment_path) noexcept {
    unsigned int vertex_shader = compile_shader(read_shader(vertex_path), GL_VERTEX_SHADER);
    unsigned int fragment_shader = compile_shader(read_shader(fragment_path), GL_FRAGMENT_SHADER);

    if (vertex_shader == 0 || fragment_shader == 0) std::abort();

    m_id = create_shader_program(vertex_shader, fragment_shader);

    glDetachShader(m_id, vertex_shader);
    glDetachShader(m_id, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (m_id == 0) std::abort();
}

Shader::~Shader() {
    glDeleteProgram(m_id);
}

void Shader::use() const {
    glUseProgram(m_id);
}

int Shader::get_location(const std::string &name) const {
    return glGetUniformLocation(m_id, name.c_str());
}

void Shader::set_int(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_uint(const std::string &name, unsigned int value) const {
    glUniform1ui(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_2f(const std::string &name, float v1, float v2) const {
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), v1, v2);
}

void Shader::set_3f(const std::string &name, float v1, float v2, float v3) const {
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), v1, v2, v3);
}

void Shader::set_4f(const std::string &name, float v1, float v2, float v3, float v4) const {
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), v1, v2, v3, v4);
}

void Shader::set_vec3f(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::set_mat3f(const std::string &name, const glm::mat3 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat4f(const std::string &name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}