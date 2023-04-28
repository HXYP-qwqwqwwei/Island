#version 460 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexUV;

out vec2 fPos;
out vec2 fTexUV;

void main() {
    gl_Position = vec4(vPos, 0, 1);
    fPos        = vPos;
    fTexUV      = vTexUV;
}