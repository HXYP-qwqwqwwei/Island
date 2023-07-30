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

out vec3 fPos;
out vec2 fTexUV;
out mat3 TBN;


void main() {
    vec4 posVec     = vModel * vec4(vPos, 1.0);
    gl_Position     = proj * view * posVec;
    mat3 model3x3   = mat3(vModel);
    fPos    = posVec.xyz;
    fTexUV  = vTexUV;

    // Tangent Space Transform
    vec3 N = normalize(model3x3 * vNormal);
    vec3 T = model3x3 * vTangent;
    T = normalize(T - dot(T, N) * N);   // Schmidt正交化
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
}
