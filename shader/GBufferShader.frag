#version 460 core

in vec3 fPos;
in vec2 fTexUV;
in mat3 TBN;

layout (location = 0) out vec4 gPos;
layout (location = 1) out vec4 gNorm;
layout (location = 2) out vec4 gDiff;
layout (location = 3) out vec4 gSpec;

struct Textures {
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normals0;
    sampler2D reflect0;
    sampler2D parallax0;
    float shininess;
};

uniform Textures texes;
uniform vec3 viewPos;

vec2 parallaxFixedUV(sampler2D parallaxTex, vec3 view);

void main() {
    gPos  = vec4(fPos, 1.0);

    vec3 view_tanSpace = transpose(TBN) * normalize(viewPos - fPos);
    vec2 fixedUV = parallaxFixedUV(texes.parallax0, view_tanSpace);

    gDiff = texture(texes.diffuse0, fixedUV);
    if (gDiff.a < 0.05) {
        discard;
    }

    vec3 norm = texture(texes.normals0, fixedUV).rgb;
    norm  = 2.0 * norm - 1.0;
    gNorm = vec4(normalize(TBN * norm), 1.0);
    gSpec = vec4(texture(texes.specular0, fixedUV).rgb, 1.0);
}


vec2 parallaxFixedUV(sampler2D parallaxTex, vec3 view) {
    float minLayers = 8;
    float maxLayers = 32;

    float nLayers = mix(minLayers, maxLayers, abs(dot(vec3(0, 0, 1), view)));
    float layerDepth = 1 / nLayers;
    vec2 dUV = -view.xy / (nLayers * view.z) * 0.1;

    vec2 uv = fTexUV;
    float depth = 1.0 - texture(parallaxTex, uv).r;
    float z = 0.0;
    while (depth > z) {
        z += layerDepth;
        uv += dUV;
        depth = 1.0 - texture(parallaxTex, uv).r;
    }
    vec2 prevUV = uv - dUV;
    float prevDepth = 1.0 - texture(parallaxTex, prevUV).r;
    float d2 = prevDepth - (z - layerDepth);
    float d1 = z - depth;
    uv -= dUV * d1 / (d1 + d2);     // 线性插值
    return uv;
}
