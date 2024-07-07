#include "scene/Components.h"

namespace component {

glm::mat4 Transform::get_matrix() const
{
    glm::mat4 output(1.0f);
    output = glm::translate(output, glm::vec3(x, y, 0.0f));
    //output = glm::rotate(output, rotation_rad, glm::vec3(0.0f, 0.0f, -1.0f));
    output = glm::scale(output, glm::vec3(sx, sy, 1.0f));
    return output;
}

}