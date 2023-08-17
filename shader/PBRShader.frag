#version 460 core

#define PI 3.1415926538f
#define MAX_CSM_LEVELS 4
#define MAX_N_PLIGHTS  4


in vec3 fPos;       // view-space
in vec2 fTexUV;
in vec3 viewVec_tanSpace;
in mat3 TBN;        // tan-space -> view-space
in vec4 fPosLiSpace[MAX_CSM_LEVELS];

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};


struct Textures {
    sampler2D diffuse0;
    sampler2D metalness0;
    sampler2D roughness0;
    sampler2D ao0;
    sampler2D normals0;
    sampler2D reflect0;
    sampler2D parallax0;
    float shininess;
};

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

uniform PointLight pointLights[MAX_N_PLIGHTS];
uniform DirectLight directLight;

uniform samplerCube environment;
uniform Textures texes;


float D_GGX_TR(vec3 n, vec3 h, float roughness);
float G_GGX_Schlick(float dot_val, float k);
float G_Smith(float dot_nv, float dot_nl, float k);
vec3 F_Schlick(float dot_hv, vec3 F0, vec3 albedo, float metalness);

void main() {
    float gamma = 2.2f;
    vec4 texDiff = texture(texes.diffuse0,  fTexUV);
    if (texDiff.a < 0.05) {
        discard;
    }
    texDiff = vec4(pow(texDiff.rgb, vec3(gamma)), texDiff.a);     // Transfer to Linear Space


    vec3 V = normalize(-fPos);
    vec3 N = texture(texes.normals0, fTexUV).xyz;
    N = N * 2.0 - 1.0;
    N = TBN * N;

    float roughness = texture(texes.roughness0, fTexUV).r;
    float metalness = texture(texes.metalness0, fTexUV).r;
    float ao = texture(texes.ao0, fTexUV).r;
    vec3 albedo = texDiff.rgb;

    vec3 Lo = vec3(0.0f);
    for (int i = 0; i < MAX_N_PLIGHTS; ++i) {
        vec3 LiPos = (view * vec4(pointLights[i].pos, 1.0)).xyz;
        vec3 L = LiPos - fPos;

        float dis = length(L);
        float Kc = pointLights[i].attenu.x;
        float Kl = pointLights[i].attenu.y;
        float Kq = pointLights[i].attenu.z;
        float attenu = 1.0 / (Kc + Kl*dis + Kq*dis*dis);
        vec3 Li = attenu * pointLights[i].color * 2.0;

        L = normalize(L);
        vec3 H = normalize(L + V);
        float dotNV = max(dot(N, V), 0.0);
        float dotNL = max(dot(N, L), 0.0);

        float a1 = roughness + 1.0;
        float k_direct = a1 * a1 / 8.0;

        float D = D_GGX_TR(N, H, roughness);
        vec3  F = F_Schlick(dot(H, V), vec3(0.04), albedo, metalness);
        float G = G_Smith(dotNV, dotNL, k_direct);

        vec3 DFG = (D * G) * F;
        vec3 f_cook_torrance = DFG / (4.0 * dotNV * dotNL + 0.001);     // avoid divided by zero
        vec3 f_lambert = albedo;

        vec3 ks = F;
        vec3 kd = (1.0 - ks) * (1.0 - metalness);

        Lo += (kd * f_lambert + ks * f_cook_torrance) * Li * dotNL;
    }

    vec3 ambient = directLight.ambient * albedo * ao;

    fragColor = vec4(ambient + Lo, 1.0);
    brightColor = vec4(0);

//    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
//    if (brightness > 10.0) {
//        brightColor = fragColor;
//    } else brightColor = vec4(0, 0, 0, 1.0);

}

// n: normal, h: halfway-vec
float D_GGX_TR(vec3 n, vec3 h, float roughness) {
    float a2 = roughness * roughness + 1e-6;
    float nh = max(dot(n, h), 0.0);
    float temp = nh * nh * (a2 - 1.0) + 1.0;
    return a2 / (PI * temp * temp);
}

float G_GGX_Schlick(float dot_val, float k) {
    return dot_val / (dot_val * (1.0-k) + k);
}

float G_Smith(float dot_nv, float dot_nl, float k) {
    return G_GGX_Schlick(dot_nv, k) * G_GGX_Schlick(dot_nl, k);
}

vec3 F_Schlick(float dot_hv, vec3 F0, vec3 albedo, float metalness) {
    F0 = mix(F0, albedo, metalness);
    float temp = 1 - dot_hv;
    float temp2 = temp * temp;
    float temp4 = temp2 * temp2;
    return F0 + (1.0 - F0) * temp * temp4;
}
