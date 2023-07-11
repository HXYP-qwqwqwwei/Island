#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexUV;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

out vec3 fPos;
out vec4 fPosLightSpace;
out vec3 fNormal;
out vec2 fTexUV;

uniform mat4 lightSpaceMtx;

void main() {
    vec4 posVec = vModel * vec4(vPos, 1.0);
    gl_Position = proj * view * posVec;
    fPos        = posVec.xyz;
    fNormal     = normalize(mat3(vModel) * vNormal);
    fTexUV      = vTexUV;
    // Light Space
    fPosLightSpace = lightSpaceMtx * vec4(fPos, 1.0);
}