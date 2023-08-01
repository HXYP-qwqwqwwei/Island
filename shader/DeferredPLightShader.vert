#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

void main() {
    gl_Position = proj * view * vModel * vec4(vPos, 1.0);
}