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

#define MAX_CSM_LEVELS 4

out vec3 fPos;      // view-space
out vec2 fTexUV;
out vec3 viewVec_tanSpace;
out vec3 color;
out mat3 TBN;       // tan-space -> view-space
out vec4 fPosLiSpace[MAX_CSM_LEVELS];

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
    sampler2D csmMaps[MAX_CSM_LEVELS];
    mat4 LiSpaceMatrices[MAX_CSM_LEVELS];
    float farDepths[MAX_CSM_LEVELS];
    int csmLevels;
};

uniform PointLight pointLights[4];
uniform DirectLight directLight;


void main() {
    mat4 ViewModel  = view * vModel;
    vec4 posVec     = ViewModel * vec4(vPos, 1.0);
    gl_Position     = proj * posVec;
    mat3 normalMtx = mat3(ViewModel);

    fPos    = posVec.xyz;
    color   = vColor;
    fTexUV  = vTexUV;
    // Light Space
    for (int i = 0; i < MAX_CSM_LEVELS; ++i) {
        fPosLiSpace[i].xyzw = directLight.LiSpaceMatrices[i] * vec4(fPos, 1.0);
    }

    // Tangent Space Transform
    vec3 N = normalize(normalMtx * vNormal);
    vec3 T = normalMtx * vTangent;
    T = normalize(T - dot(T, N) * N);   // Schmidt正交化
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
    mat3 TBN_inverse    = transpose(TBN); // ViewSpace -> TanSpace
    viewVec_tanSpace    = TBN_inverse * (-fPos);
}
