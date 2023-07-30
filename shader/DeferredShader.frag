#version 460 core
in vec2 fTexUV;

uniform sampler2D texture0; // pos
uniform sampler2D texture1; // norm
uniform sampler2D texture2; // diff
uniform sampler2D texture3; // spec

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct PointLight {
    vec3 pos;
    vec3 color;
    float linear;
    float zFar;
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

uniform vec3 viewPos;
uniform mat4 lightSpaceMtx;

vec2 parallaxFixedUV(sampler2D parallaxTex, vec3 view);
float pointLightShadow(samplerCube depthTex, vec3 fPos, vec3 lightPos, vec3 norm, float zFar);
float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm);


void main() {
    float gamma = 2.2;
    vec3 fPos    = texture(texture0, fTexUV).xyz;
    vec3 texNorm = texture(texture1, fTexUV).xyz;

    vec3 texDiff = texture(texture2, fTexUV).rgb;
    texDiff = pow(texDiff, vec3(gamma));

    vec3 texSpec = texture(texture3, fTexUV).rgb;

    vec3 view = normalize(viewPos - fPos);

    vec3 ambient = directLight.ambient * texDiff;

    vec3 diffuse    = vec3(0.0);
    vec3 specular   = vec3(0.0);

    float dShadow = directLightShadow(directLight.depthTex, lightSpaceMtx * vec4(fPos, 1.0), directLight.injection, texNorm);
    vec3 inj_dLight  = normalize(directLight.injection);
    vec3 diff_dLight = directLight.color * max(0.0, dot(-inj_dLight, texNorm));
    diff_dLight *= (1.0 - dShadow);
    diffuse += diff_dLight;

    float shin          = max(7.82e-3, 2.0);     // 0.00782 * 128 ~= 1
    vec3 halfway_dLight = normalize(view - inj_dLight);
    vec3 spec_dLight    = directLight.color * pow(max(dot(halfway_dLight, texNorm), 0.0), shin * 128);
    specular += spec_dLight;

    for (int i = 0; i < 4; ++i) {
        float pShadow = pointLightShadow(pointLights[i].depthTex, fPos, pointLights[i].pos, texNorm, pointLights[i].zFar);
        vec3 inj_pLight = fPos - pointLights[i].pos;
        float dis = length(inj_pLight);
        float attenuation = 1 / (dis * dis * pointLights[i].linear);
        inj_pLight /= dis;
        vec3 diff_pLight = pointLights[i].color * (max(dot(-inj_pLight, texNorm), 0.0) * attenuation);
        diff_pLight *= (1.0 - pShadow);

        vec3 halfway_pLight = normalize(view - inj_pLight);
        vec3 spec_pLight = pointLights[i].color * (pow(max(dot(halfway_pLight, texNorm), 0.0), shin * 128) * attenuation);
        spec_pLight *= (1.0 - pShadow);

        diffuse += diff_pLight;
        specular += spec_pLight;
    }

    diffuse  *= texDiff.rgb;
    specular *= texSpec.rgb;

    vec3 color = ambient + diffuse + specular;
    fragColor = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);

}



vec3 cubeSampleOffsets[20] = {
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
};

float pointLightShadow(samplerCube depthTex, vec3 fPos, vec3 lightPos, vec3 norm, float zFar) {
    vec3 injection = fPos - lightPos;
    float currDepth = length(injection);

    // PCF
    float bias = max(0.04 * (1.0 - dot(norm, -normalize(injection))), 0.0003);
    float shadow = 0;
    for (int i = 0; i < 20; ++i) {
        float depth = texture(depthTex, injection + cubeSampleOffsets[i] * 0.005).r;
        depth *= zFar;
        shadow += (currDepth - bias) > depth ? 1.0 : 0.0;
    }
    return shadow / 20;
}


float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm) {
    vec3 projCoords = fPosLSpace.xyz / fPosLSpace.w;
    // 将坐标范围从[-1, 1]映射到[0, 1]
    projCoords = (projCoords + 1) * 0.5;
    float currDepth = projCoords.z;
    float bias = max(0.0001 * (1.0 - dot(norm, -normalize(injction))), 0.00001);
    vec2 texSize = textureSize(depthTex, 0);

    float shadow = 0;

    // PCF
    int offset = 3;
    float dx = 1 / texSize.x;
    float dy = 1 / texSize.y;
    for (int i = -offset; i <= offset; ++i) {
        for (int j = -offset; j <= offset; ++j) {
            float depth = texture(depthTex, vec2(projCoords.x + i * dx, projCoords.y + j * dy)).r;
            shadow += (currDepth - bias) > depth ? 1.0 : 0.0;
        }
    }
    int kernelWidth = 2*offset + 1;
    shadow /= (kernelWidth * kernelWidth);

    return shadow;
}