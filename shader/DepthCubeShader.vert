#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 2) in vec2 vTexUV;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6

out G_IN {
    vec2 gTexUV;
} v_out;

void main() {
    v_out.gTexUV = vTexUV;
    gl_Position = vModel * vec4(vPos, 1.0);
}