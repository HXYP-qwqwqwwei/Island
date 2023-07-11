#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 2) in vec2 vTexUV;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6


out vec2 fTexUV;

uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * vModel * vec4(vPos, 1.0);
    fTexUV      = vTexUV;
}