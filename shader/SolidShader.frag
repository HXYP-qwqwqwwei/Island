#version 460 core

in vec3 fPos;
in vec4 fPosLightSpace;
in vec3 fNormal;
in vec2 fTexUV;
in vec3 pLightInj_tanSpace[4];
in vec3 dLightInj_tanSpace;
in vec3 viewVec_tanSpace;
in mat3 TBN;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct PointLight {
    vec3 pos;
    vec3 color;
    float linear;
    float zFar;
    samplerCube shadowMap;
};

struct DirectLight {
    vec3 color;
    vec3 ambient;
    sampler2D shadowMap;
};

#define N_TEXTURE 4
struct Textures {
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normals0;
    sampler2D reflect0;
    sampler2D parallax0;
    float shininess;
};

uniform samplerCube environment;
uniform PointLight pointLights[4];
uniform DirectLight directLight;
uniform Textures texes;
uniform vec3 directLightInjection;


vec2 parallaxFixedUV(vec3 view);
float pLightShadow(vec3 fPos, vec3 lightPos, samplerCube shadowMap, float zFar);
float dLightShadow(vec4 fPosLSpace, vec3 lightInjction);


void main() {
    vec3 view    = normalize(viewVec_tanSpace);
    vec2 fixedUV = parallaxFixedUV(view);

    vec4 texDiff = texture(texes.diffuse0,  fixedUV);
    texDiff = vec4(pow(texDiff.rgb, vec3(2.2)), texDiff.a);     // Transfer to Linear Space

    vec4 texSpec = texture(texes.specular0, fixedUV);
    vec4 texRfle = texture(texes.reflect0,  fixedUV);
    vec3 texNorm = texture(texes.normals0,  fixedUV).rgb;
    texNorm = normalize(texNorm * 2.0 - 1.0);  // IMPORTANT!!! 需要把坐标从[0, 1]映射到[-1, +1]


    // ambient
    vec3 ambient = directLight.ambient * texDiff.rgb;


    // directional light
    vec3 diffuse_dLight = directLight.color * max(0.0f, dot(-dLightInj_tanSpace, texNorm));
    float dShadow       = dLightShadow(fPosLightSpace, directLightInjection);
    diffuse_dLight     *= (1 - dShadow);
    vec3 diffuse        = diffuse_dLight;

    // specular: Blinn-Phong
    float shin          = max(7.82e-3, texes.shininess);     // 0.00782 * 128 ~= 1
    vec3 halfway_dLight = normalize(-dLightInj_tanSpace + view);
    vec3 spec_dLight    = pow(max(dot(texNorm, halfway_dLight), 0.0f), shin * 128) * directLight.color;
    spec_dLight        *= (1 - dShadow);
    vec3 specular       = spec_dLight;


    // point light
    for (int i = 0; i < 4; ++i) {
        float pShadow       = pLightShadow(fPos, pointLights[i].pos, pointLights[i].shadowMap, pointLights[i].zFar);
        vec3 inj_pLight     = pLightInj_tanSpace[i].xyz;
        float lightDis      = length(pLightInj_tanSpace[i]);
        float attenuation   = 1.0 / (lightDis * lightDis * pointLights[i].linear);   // at linear space
        vec3 pLightResult   = pointLights[i].color * attenuation;
        inj_pLight          = normalize(inj_pLight);
        vec3 diffuse_pLight = pLightResult * max(0.0f, dot(-inj_pLight, texNorm));
        diffuse_pLight     *= (1 - pShadow);
        diffuse += diffuse_pLight;

        // specular
        vec3 halfway_pLight = normalize(-inj_pLight + view);
        vec3 spec_pLight    = pow(max(dot(texNorm, halfway_pLight), 0.0f), shin * 128) * pLightResult;
        spec_pLight        *= (1 - pShadow);
        specular += spec_pLight;
    }

    specular *= texSpec.rgb;
    diffuse  *= texDiff.rgb;


    // reflection
    vec3 refl = reflect(-view, texNorm);
    vec3 reflection = texture(environment, TBN * refl).rgb * texRfle.rgb;

    fragColor = vec4(ambient + diffuse + specular + reflection, 1);


    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);
}


vec2 parallaxFixedUV(vec3 view) {
    float minLayers = 8;
    float maxLayers = 32;

    float nLayers = mix(minLayers, maxLayers, abs(dot(vec3(0, 0, 1), view)));
    float layerDepth = 1 / nLayers;
    vec2 dUV = -view.xy / (nLayers * view.z) * 0.1;

    vec2 uv = fTexUV;
    float depth = 1.0 - texture(texes.parallax0, uv).r;
    float z = 0.0;
    while (depth > z) {
        z += layerDepth;
        uv += dUV;
        depth = 1.0 - texture(texes.parallax0, uv).r;
    }
    vec2 prevUV = uv - dUV;
    float prevDepth = 1.0 - texture(texes.parallax0, prevUV).r;
    float d2 = prevDepth - (z - layerDepth);
    float d1 = z - depth;
    uv -= dUV * d1 / (d1 + d2);     // 线性插值
    return uv;
}


vec3 cubeSampleOffsets[20] = {
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
};

float pLightShadow(vec3 fPos, vec3 lightPos, samplerCube shadowMap, float zFar) {
    vec3 injection = fPos - lightPos;
    float currDepth = length(injection);

    // PCF
    float bias = max(0.04 * (1.0 - dot(fNormal, -normalize(injection))), 0.0003);
    float shadow = 0;
    for (int i = 0; i < 20; ++i) {
        float depth = texture(shadowMap, injection + cubeSampleOffsets[i] * 0.005).r;
        depth *= zFar;
        shadow += (currDepth - bias) > depth ? 1.0 : 0.0;
    }
    return shadow / 20;
}


float dLightShadow(vec4 fPosLSpace, vec3 lightInjction) {
    vec3 projCoords = fPosLSpace.xyz / fPosLSpace.w;
    // 将坐标范围从[-1, 1]映射到[0, 1]
    projCoords = (projCoords + 1) * 0.5;
    float currDepth = projCoords.z;
    float bias = max(0.001 * (1.0 - dot(fNormal, -lightInjction)), 0.0001);
    vec2 texSize = textureSize(directLight.shadowMap, 0);

    float shadow = 0;

    // PCF
    int offset = 3;
    float dx = 1 / texSize.x;
    float dy = 1 / texSize.y;
    for (int i = -offset; i <= offset; ++i) {
        for (int j = -offset; j <= offset; ++j) {
            float depth = texture(directLight.shadowMap, vec2(projCoords.x + i * dx, projCoords.y + j * dy)).r;
            shadow += (currDepth - bias) > depth ? 1.0 : 0.0;
        }
    }
    int kernelWidth = 2*offset + 1;
    shadow /= (kernelWidth * kernelWidth);

    return shadow;
}