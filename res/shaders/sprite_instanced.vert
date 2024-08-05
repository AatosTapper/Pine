#version 410 core 

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;
layout (location = 2) in mat4 a_transform;

uniform mat4 u_view_proj;

out vec2 tex_coord;

void main() {
    vec4 pos = vec4(a_pos, 1.0);
    gl_Position = u_view_proj * a_transform * pos;
    tex_coord = a_tex_coord;
}