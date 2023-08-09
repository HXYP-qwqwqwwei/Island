#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

struct PointLight {
    vec3 pos;
    vec3 color;
    vec3 attenu;
    vec2 zNearFar;
    samplerCube depthTex;
};

uniform PointLight pointLight;

out vec3 pLightPos_viewSpace;
out mat3 world;


void main() {
    gl_Position = proj * view * vModel * vec4(vPos, 1.0);
    pLightPos_viewSpace = (view * vec4(pointLight.pos, 1.0)).xyz;
    world = transpose(mat3(view));
}