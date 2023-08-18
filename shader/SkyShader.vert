#version 460 core
layout (location = 0) in vec3 vPos;

out vec3 fTexUVW;

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

void main() {
    fTexUVW = vPos;
    vec4 pos = proj * vec4(mat3(view) * vPos, 1);
    gl_Position = pos.xyww;     // ensure Z-buffer == 1.0
}