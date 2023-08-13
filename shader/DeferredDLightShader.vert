#version 460 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexUV;

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

out vec2 fTexUV;
out mat3 view_inv3x3;
out vec3 dLightInj_viewSpace;

#define MAX_CSM_LEVELS 4

struct DirectLight {
    vec3 injection;
    vec3 color;
    vec3 ambient;
    sampler2D csmMaps[MAX_CSM_LEVELS];
    mat4 LiSpaceMatrices[MAX_CSM_LEVELS];
    float farDepths[MAX_CSM_LEVELS];
    int csmLevels;
};

uniform DirectLight directLight;

void main() {
    gl_Position = vec4(vPos, 0, 1);
    fTexUV      = vTexUV;
    view_inv3x3 = mat3(view);
    dLightInj_viewSpace = mat3(view) * directLight.injection;
}