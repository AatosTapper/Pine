#version 410 core

out vec4 frag_color;
in vec2 tex_coord;

uniform sampler2D tex_col;
uniform sampler2D tex_dep;

vec4 aces(vec4 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return min(max((x * (a * x + b)) / (x * (c * x + d) + e), 0.0), 1.0);
}

void main() {
    frag_color = aces(texture(tex_col, tex_coord));
}