#pragma once

#include "config.h"
#include "pch.h"

#define PROJ_2D true
#define PROJ_3D false

class Camera {
public:
    Camera(const float aspect_ratio, const float field_of_view, bool ortho) noexcept;

    float pitch;
    float yaw;
    float fov;

    void update_last_pos();
    void update(float interp_factor = 0.0f);
    
    void set_aspect_ratio(const float aspect_ratio) { m_aspect_ratio = aspect_ratio; }
    glm::mat4 get_vp_matrix() const { return *m_vp_mat; }
    const glm::vec3 &get_position() const { return m_position; }
    void set_position(const glm::vec3 &new_pos) { m_position = new_pos; }

    // move
    void forward(const float amount);
    void back(const float amount);
    void left(const float amount);
    void right(const float amount);
    void up(const float amount);
    void down(const float amount);

private:
    glm::mat4 m_view;
    glm::vec3 m_position;
    glm::vec3 m_last_position; // used for smooth frame interpolation
    glm::vec3 m_direction; // points in the reverse direction

    glm::vec3 m_front;
    glm::vec3 m_right;
    glm::vec3 m_up;
    std::unique_ptr<glm::mat4> m_vp_mat;
    float m_aspect_ratio = 1.0f;

    const bool m_ortho;

    void m_update_direction();
};