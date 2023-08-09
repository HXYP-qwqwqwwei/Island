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

out vec3 fPos;      // view space
out vec2 fTexUV;


void main() {
    fTexUV  = vTexUV;

    mat4 ViewModel  = view * vModel;
    vec4 posVec     = ViewModel * vec4(vPos, 1.0);
    gl_Position     = proj * posVec;
    fPos    = posVec.xyz;

    // Tangent Space Transform
    mat3 normalMtx  = mat3(ViewModel);
    vec3 N = normalize(normalMtx * vNormal);
    vec3 T = normalMtx * vTangent;
    T = normalize(T - dot(T, N) * N);   // Schmidt正交化
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);
}
