#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6           - [Instanced Array]
layout (location = 8) in vec3 center;   // center pos                     - [Instanced Array]
layout (location = 9) in vec3 color;    // color                          - [Instanced Array]
layout (location = 10) in vec3 attenu;  // constant, linear, quadratic    - [Instanced Array]
layout (location = 11) in vec2 zNearFar; // zNear, zFar                   - [Instanced Array]

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

struct PointLight {
    vec3 pos;
    vec3 color;
    vec3 attenu;
    vec2 zNearFar;
};

out PointLight pointLight;

void main() {
    gl_Position     = proj * view * vModel * vec4(vPos, 1.0);
    pointLight.zNearFar = zNearFar;
    pointLight.color    = color;
    pointLight.pos      = (view * vec4(center, 1.0)).xyz;
    pointLight.attenu   = attenu;
}
