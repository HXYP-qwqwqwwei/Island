#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexUV;
layout (location = 3) in mat4 vModel;   // location: 3, 4, 5, 6           - [Instanced Array]
layout (location = 7) in vec3 vTangent;
layout (location = 9) in vec3 vColor;    // color                         - [Instanced Array]


layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

out vec3 fPos;
out vec4 fPosLightSpace;
out vec2 fTexUV;
out vec3 pLightInj_tanSpace[4];
out vec3 dLightInj_tanSpace;
out vec3 viewVec_tanSpace;
out vec3 color;
out mat3 TBN;

uniform mat4 lightSpaceMtx;
uniform vec3 viewPos;


struct PointLight {
    vec3 pos;
    vec3 color;
    vec3 attenu;
    vec2 zNearFar;
    samplerCube depthTex;
};

struct DirectLight {
    vec3 injection;
    vec3 color;
    vec3 ambient;
    sampler2D depthTex;
};

uniform PointLight pointLights[4];
uniform DirectLight directLight;


void main() {
    vec4 posVec     = vModel * vec4(vPos, 1.0);
    gl_Position     = proj * view * posVec;
    mat3 model3x3   = mat3(vModel);
    fPos    = posVec.xyz;
    color   = vColor;
    fTexUV  = vTexUV;
    // Light Space
    fPosLightSpace = lightSpaceMtx * vec4(fPos, 1.0);

    // Tangent Space Transform
    vec3 N = normalize(model3x3 * vNormal);
    vec3 T = model3x3 * vTangent;
    T = normalize(T - dot(T, N) * N);   // Schmidt正交化
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
    mat3 TBN_inverse    = transpose(mat3(TBN));
    for (int i = 0; i < 4; ++i) {
        pLightInj_tanSpace[i].xyz = TBN_inverse * (fPos - pointLights[i].pos);
    }
    dLightInj_tanSpace  = normalize(TBN_inverse * directLight.injection);
    viewVec_tanSpace    = TBN_inverse * (viewPos - fPos);
}
