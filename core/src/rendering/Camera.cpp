#include "Camera.h"

#define WORLD_UP glm::vec3(0.0f, 1.0f, 0.0f)

Camera::Camera(const float aspect_ratio, const float field_of_view, bool ortho) noexcept : 
    pitch(0.0f), yaw(-90.0f),
    fov(field_of_view),
    m_position(glm::vec3(0.0f)), 
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)), 
    m_delta_pos(glm::vec3(0.0f)),
    m_vp_mat(std::make_unique<glm::mat4>(1.0f)),
    m_ortho(ortho)
{
    set_aspect_ratio(aspect_ratio);
    update();
}

void Camera::update() {
    m_position += m_delta_pos;
    m_delta_pos = { 0.0f, 0.0f, 0.0f };
    m_update_direction();
    m_front = m_direction;
    m_up = glm::normalize(glm::cross(m_direction, glm::normalize(glm::cross(WORLD_UP, m_direction))));
    m_right = glm::normalize(glm::cross(m_front, m_up));
    m_view = glm::lookAt(m_position, m_position + m_front, m_up);
    
    glm::mat4 projection;
    if (m_ortho) projection = glm::ortho(-fov * m_aspect_ratio, fov * m_aspect_ratio, -fov, fov, 0.1f, 1000.0f);
    else projection = glm::perspective(glm::radians(fov), m_aspect_ratio, 0.1f, 1000.0f);

    *m_vp_mat = projection * m_view;
}

void Camera::forward(const float amount) {
    m_delta_pos += amount * m_front;
}

void Camera::back(const float amount) {
    m_delta_pos -= amount * m_front;
}

void Camera::left(const float amount) {
    m_delta_pos -= amount * m_right;
}

void Camera::right(const float amount) {
    m_delta_pos += amount * m_right;
}

void Camera::up(const float amount) {
    m_delta_pos += amount * WORLD_UP;
}

void Camera::down(const float amount) {
    m_delta_pos -= amount * WORLD_UP;
}

void Camera::m_update_direction() {
    m_direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    m_direction.y = std::sin(glm::radians(pitch));
    m_direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    m_direction = glm::normalize(m_direction);
}