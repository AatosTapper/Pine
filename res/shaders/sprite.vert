#version 410 core 

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coord;

uniform mat4 u_view_proj;
uniform mat4 u_transform;
uniform float u_z_fight_factor;

out vec2 tex_coord;

void main() {
    vec4 pos = vec4(vec3(vec2(a_pos.x, a_pos.y), u_z_fight_factor), 1.0);
    gl_Position = u_view_proj * u_transform * pos;
    tex_coord = a_tex_coord;
}