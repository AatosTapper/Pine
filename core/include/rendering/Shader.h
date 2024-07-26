#pragma once

#include "config.h"
#include "pch.h"
#include "singleton.h"

class Shader {
public:
    Shader(const std::string &vertex_path, const std::string &fragment_path) noexcept;
    ~Shader();
    
    IMPL_NO_COPY(Shader)

    void use() const;
    unsigned int get_id() const { return m_id; }

    // uniform setting
    int get_location(const std::string &name) const;
    void set_int(const std::string &name, int value) const;
    void set_uint(const std::string &name, unsigned int value) const;
    void set_float(const std::string &name, float value) const;
    void set_2f(const std::string &name, float v1, float v2) const;
    void set_3f(const std::string &name, float v1, float v2, float v3) const;
    void set_4f(const std::string &name, float v1, float v2, float v3, float v4) const;
    void set_vec3f(const std::string &name, const glm::vec3 &value) const;
    void set_mat3f(const std::string &name, const glm::mat3 &value) const;
    void set_mat4f(const std::string &name, const glm::mat4 &value) const;

private:
    unsigned int m_id = 0;
};