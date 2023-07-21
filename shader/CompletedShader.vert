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
out vec4 fPosLightSpace;
out vec3 fNormal;
out vec2 fTexUV;
out vec3 pLightInj_tanSpace[4];
out vec3 dLightInj_tanSpace;
out vec3 viewVec_tanSpace;
out mat3 TBN;

uniform mat4 lightSpaceMtx;
uniform vec3 viewPos;
uniform vec3 directLightInjection;


struct PointLight {
    vec3 pos;
    vec3 color;
    float linear;
    float zFar;
    samplerCube shadowMap;
};

uniform PointLight pointLights[4];


void main() {
    vec4 posVec     = vModel * vec4(vPos, 1.0);
    gl_Position     = proj * view * posVec;
    mat3 model3x3   = mat3(vModel);
    fPos     = posVec.xyz;
    fNormal  = normalize(model3x3 * vNormal);
    fTexUV   = vTexUV;
    // Light Space
    fPosLightSpace = lightSpaceMtx * vec4(fPos, 1.0);

    // Tangent Space Transform
    vec3 T = model3x3 * vTangent;
    T = normalize(T - dot(T, fNormal) * fNormal);   // Schmidt正交化
    vec3 B = cross(fNormal, T);

    TBN = mat3(T, B, fNormal);
    mat3 TBN_inverse    = transpose(mat3(TBN));
    for (int i = 0; i < 4; ++i) {
        pLightInj_tanSpace[i].xyz = TBN_inverse * (fPos - pointLights[i].pos);
    }
    dLightInj_tanSpace  = normalize(TBN_inverse * directLightInjection);
    viewVec_tanSpace    = TBN_inverse * (viewPos - fPos);
}
