#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexUV;

out vec3 fPos;
out vec3 fNormal;
out vec2 fTexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main() {
    vec4 posVec = model * vec4(vPos, 1.0);
    gl_Position = proj * view * posVec;
    fPos        = posVec.xyz;
    fNormal     = normalize(mat3(model) * vNormal);
    fTexUV      = vTexUV;
}