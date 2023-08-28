#version 460 core
in vec3 fTexUVW;

#define PI      3.1415926538f
#define TWO_PI  PI * 2.0
#define HALF_PI PI / 2.0

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 xi, vec3 N, float roughness);
float D_GGX_TR(vec3 n, vec3 h, float roughness);

uniform samplerCube texture0;
uniform uint nSamples = 4096u;
uniform float roughness;

const vec3 up = vec3(0.0, 1.0, 0.0);

out vec3 prefiltered;


void main() {
    vec3 N = normalize(fTexUVW);
    vec3 R = N;
    vec3 V = R;

    prefiltered = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0; i < nSamples; ++i) {
        vec2 xi = Hammersley(i, nSamples);
        vec3 H = ImportanceSampleGGX(xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float dotNL = max(dot(N, L), 0.0);

        float dotNH = max(dot(H, H), 0.0);
        float dotVH = dot(V, H);
        float D = D_GGX_TR(N, H, roughness);
        float pdf = (D * dotNH / (4.0 * dotVH)) + 0.0001;

        float texLen = textureSize(texture0, 0).x;
        float saTexel  = 4.0 * PI / (6.0 * texLen * texLen);
        float saSample = 1.0 / (float(nSamples) * pdf + 0.0001);
        float lod = (roughness == 0.0 ? 0.0 : 0.5 * log2(saSample/saTexel));

        prefiltered += textureLod(texture0, L, lod).rgb * dotNL;
        totalWeight += dotNL;
    }

    prefiltered /= totalWeight;
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

// n: normal, h: halfway-vec
float D_GGX_TR(vec3 n, vec3 h, float roughness) {
    float a2 = roughness * roughness + 1e-6;
    float nh = max(dot(n, h), 0.0);
    float temp = nh * nh * (a2 - 1.0) + 1.0;
    return a2 / (PI * temp * temp);
}

