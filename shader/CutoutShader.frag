#version 460 core

in vec3 fPos;
in vec4 fPosLightSpace;
in vec3 fNormal;
in vec2 fTexUV;
in vec3 pLightPos_tanSpace;
in vec3 dLightInj_tanSpace;
in vec3 fragPos_tanSpace;
in vec3 viewPos_tanSpace;

out vec4 fragColor;

struct PointLight {
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
    float shininess;
};

uniform samplerCube environment;
uniform PointLight pointLight;
uniform DirectLight directLight;
uniform Textures texes;
uniform vec3 directLightInjection;
uniform vec3 pointLightPosition;


float dLightShadow(vec4 fPosLSpace, vec3 lightInjction);
float pLightShadow(vec3 fPos, vec3 lightPos);


void main() {
    vec4 texDiff = texture(texes.diffuse0,  fTexUV);
    if (texDiff.a < 0.05) {
        discard;
    }
    vec4 texSpec = texture(texes.specular0, fTexUV);
    vec4 texRfle = texture(texes.reflect0,  fTexUV);
    vec3 texNorm = texture(texes.normals0,  fTexUV).rgb;
    texNorm = texNorm * 2 - 1;  // IMPORTANT!!! 需要把坐标从[0, 1]映射到[-1, +1]

    // ambient
    vec3 ambient = directLight.ambient * texDiff.rgb;

    // directional light
    vec3 diffuse_dLight = directLight.color * texDiff.rgb * max(0.0f, dot(-dLightInj_tanSpace, texNorm));
    float dShadow       = dLightShadow(fPosLightSpace, directLightInjection);
    diffuse_dLight     *= (1 - dShadow);


    // point light
    vec3 inj_pLight     = fragPos_tanSpace - pLightPos_tanSpace;
    float lightDis      = length(inj_pLight);
    float attenuation   = 1.0 / (1.0 + lightDis * pointLight.linear);   // at linear space
    vec3 plightResult   = pointLight.color * attenuation;
    inj_pLight          = normalize(inj_pLight);
    vec3 diffuse_pLight = plightResult * texDiff.rgb * max(0.0f, dot(-inj_pLight, texNorm));
    float pShadow       = pLightShadow(fPos, pointLightPosition);
    diffuse_pLight     *= (1 - pShadow);

    // diffuse
    vec3 diffuse = diffuse_dLight + diffuse_pLight;

    // specular
    float shin          = max(7.82e-3, texes.shininess);     // 0.00782 * 128 ~= 1
    vec3 view           = normalize(viewPos_tanSpace - fragPos_tanSpace);

    // Blinn-Phong
    vec3 halfway_dLight = normalize(-dLightInj_tanSpace + view);
    vec3 spec_dLight    = pow(max(dot(texNorm, halfway_dLight), 0.0f), shin * 128) * directLight.color;
    spec_dLight        *= (1 - dShadow);

    vec3 halfway_pLight = normalize(-inj_pLight + view);
    vec3 spec_pLight    = pow(max(dot(texNorm, halfway_pLight), 0.0f), shin * 128) * plightResult;
    spec_pLight        *= (1 - pShadow);

    vec3 specular       = (spec_pLight + spec_dLight) * texSpec.rgb;

    fragColor = vec4(ambient + diffuse + specular, 1);
}

vec3 cubeSampleOffsets[20] = {
vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
};

float pLightShadow(vec3 fPos, vec3 lightPos) {
    vec3 injection = fPos - lightPos;

    float currDepth = length(injection);

    float bias = max(0.01 * (1.0 - dot(fNormal, -normalize(injection))), 0.0003);
    float shadow = 0;
    for (int i = 0; i < 20; ++i) {
        float depth = texture(pointLight.shadowMap, injection + cubeSampleOffsets[i] * 0.005).r;
        depth *= pointLight.zFar;
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