#version 460 core

in vec3 fPos;
in vec4 fPosLightSpace;
in vec3 fNormal;
in vec2 fTexUV;

out vec4 fragColor;

struct SpotLight {
    vec3 color;
    vec3 pos;
    vec3 direction;
    float linear;
    float zFar;
    float cutOff;
};

struct PointLight {
    vec3 color;
    vec3 pos;
    float linear;
    float zFar;
    samplerCube shadowMap;
};

struct DirectLight {
    vec3 injection;
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
//uniform samplerCube shadowMap;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform DirectLight directLight;
uniform Textures texes;
uniform vec3 viewPos;

float dLightShadow(vec4 fPosLSpace, vec3 lightInjction);
float pLightShadow(vec3 fPos);


void main() {
    vec4 texDiff = texture(texes.diffuse0, fTexUV);
    vec4 texSpec = texture(texes.specular0, fTexUV);
    vec4 texNorm = texture(texes.normals0, fTexUV);
    vec4 texRfle = texture(texes.reflect0, fTexUV);
    // ambient
    vec3 ambient = directLight.ambient * texDiff.rgb;

    // directional light
    vec3 inj_dLight     = normalize(directLight.injection);
    vec3 diffuse_dLight = directLight.color * texDiff.rgb * max(0.0f, dot(-inj_dLight, fNormal));
    float dShadow = dLightShadow(fPosLightSpace, inj_dLight);
    diffuse_dLight *= (1 - dShadow);

    // TODO sopt light

    // point light
    float lightDis      = distance(pointLight.pos, fPos);
    float attenuation = 1.0 / (lightDis * pointLight.linear);   // at linear space
    vec3 plightResult   = pointLight.color * attenuation;
    vec3 inj_pLight     = normalize(fPos - pointLight.pos);
    vec3 diffuse_pLight = plightResult * texDiff.rgb * max(0.0f, dot(-inj_pLight, fNormal));
    float pShadow       = pLightShadow(fPos);
    diffuse_pLight *= (1 - pShadow);
//    diffuse_pLight = vec3(pShadow);

    // diffuse
    vec3 diffuse = diffuse_dLight + diffuse_pLight;

    // specular
    float shin          = max(7.82e-3, texes.shininess);     // 0.00782 * 128 ~= 1
    vec3 view           = normalize(viewPos - fPos);
    // Phong
    //    vec3 ref_pLight     = reflect(inj_pLight, fNormal);
    //    vec3 ref_dLight     = reflect(inj_dLight, fNormal);
    //    vec3 spec_pLight    = pow(max(dot(view, ref_pLight), 0.0f), shin * 128) * plightResult;
    //    vec3 spec_dLight    = pow(max(dot(view, ref_dLight), 0.0f), shin * 128) * directLight.color;
    // Blinn-Phong
    vec3 halfway_dLight = normalize(-inj_dLight + view);
    vec3 spec_dLight    = pow(max(dot(fNormal, halfway_dLight), 0.0f), shin * 128) * directLight.color;
    spec_dLight *= (1 - dShadow);
    vec3 halfway_pLight = normalize(-inj_pLight + view);
    vec3 spec_pLight    = pow(max(dot(fNormal, halfway_pLight), 0.0f), shin * 128) * plightResult;
    spec_pLight *= (1 - pShadow);

    vec3 specular       = (spec_pLight + spec_dLight) * texSpec.rgb;

    // reflection
    //    vec3 rayTrace       = -view;
    //    vec3 reflectionVec  = reflect(rayTrace, fNormal);
    //    vec3 reflection     = texture(environment, reflectionVec).rgb * texRfle.rgb;

    //    shadow = 1.0f;

    fragColor = vec4(ambient + diffuse + specular, 1);
    //    fragColor = vec4(vec3(gl_FragCoord.z), 1.0);

    //    fragColor = vec4(reflection, 1);
}

vec3 cubeSampleOffsets[20] = {
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
};

float pLightShadow(vec3 fPos) {
    vec3 injection = fPos - pointLight.pos;

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
    // 将坐标范围从[-1, 1]转换到[0, 1]
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
    int kernelWidth = 2* offset + 1;
    shadow /= (kernelWidth * kernelWidth);

    return shadow;
}