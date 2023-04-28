#version 460 core
layout (location = 0) in vec3 vPos;

out vec3 fTexUVW;

uniform mat4 proj;
uniform mat4 view;

void main() {
    fTexUVW = vPos;
    vec4 pos = proj * view * vec4(vPos, 1);
    gl_Position = pos.xyww;
}