#version 460 core
in vec2 fTexUV;

#define PI      3.1415926538f
#define TWO_PI  PI * 2.0
#define HALF_PI PI / 2.0

float D_GGX_TR(vec3 n, vec3 h, float roughness);
float G_GGX_Schlick(float dot_val, float k);
float G_Smith(float dot_nv, float dot_nl, float k);
vec3 F_Schlick(float dot_hv, vec3 albedo, float metalness);

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 xi, vec3 N, float roughness);

uniform uint nSamples = 1024u;

const vec3 up = vec3(0.0, 1.0, 0.0);

out vec2 FG;


void main() {
    float dotNV = fTexUV.x;
    float roughness = fTexUV.y;
    vec3 V = vec3(sqrt(1.0 - dotNV*dotNV), 0.0, dotNV);
    vec3 N = vec3(0.0, 0.0, 1.0);

    float F = 0.0;
    float G = 0.0;

    for (uint i = 0; i < nSamples; ++i) {
        vec2 xi = Hammersley(i, nSamples);
        vec3 H = ImportanceSampleGGX(xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float dotNL = max(L.z, 0.0);
        float dotNH = max(H.z, 0.0);
        float dotVH = max(dot(V, H), 0.0);

        float temp = 1.0 - dotVH;
        float temp2 = temp * temp;
        float temp5 = temp2 * temp2 * temp;

        float k_IBL = roughness * roughness / 2.0;
        float Gi = G_Smith(dotNV, dotNL, k_IBL) * dotVH / (dotNH * dotNV);
        F += (1.0 - temp5) * Gi;
        G += temp5 * Gi;
    }

    FG = vec2(F, G) / nSamples;
}

// output range: [0.0, 1.0]
float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// output xy: [0.0, 1.0]
vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 xi, vec3 N, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a + 1e-6;
    float phi = TWO_PI * xi.x;

    float cosTheta = sqrt((1.0 - xi.y) / ((a2 - 1.0) * xi.y + 1.0));    // result from CDF of D_GGX
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);  // in tangent-space

    vec3 T = normalize(cross(up, N));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * H);
}


float G_GGX_Schlick(float dot_val, float k) {
    return dot_val / (dot_val * (1.0-k) + k);
}

float G_Smith(float dot_nv, float dot_nl, float k) {
    return G_GGX_Schlick(dot_nv, k) * G_GGX_Schlick(dot_nl, k);
}

