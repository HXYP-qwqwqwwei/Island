#version 460 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexUV;

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

out vec2 fTexUV;
out mat4 view_inv;
out vec3 dLightInj_viewSpace;

struct DirectLight {
    vec3 injection;
    vec3 color;
    vec3 ambient;
    sampler2D depthTex;
};

uniform DirectLight directLight;

void main() {
    gl_Position = vec4(vPos, 0, 1);
    fTexUV      = vTexUV;
    view_inv    = inverse(view);
    dLightInj_viewSpace = mat3(view) * directLight.injection;
}