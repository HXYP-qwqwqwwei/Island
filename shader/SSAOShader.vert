#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexUV;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6
layout (location = 7) in vec3 vTangent;

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

out vec3 fPos_viewSpace;
out vec2 fTexUV;

// 观察空间
void main() {
    mat4 ViewModel  = view * vModel;
    vec4 posVec     = ViewModel * vec4(vPos, 1.0);
    gl_Position     = proj * posVec;
    fPos_viewSpace  = posVec.xyz;
    fTexUV          = vTexUV;

    mat3 normalMtx  = mat3(transpose(inverse(ViewModel)));
    vec3 N = normalMtx * vNormal;
}
