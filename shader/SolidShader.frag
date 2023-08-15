#version 460 core

#define MAX_CSM_LEVELS 4

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


#define N_TEXTURE 4
struct Textures {
    sampler2D diffuse0;
    sampler2D specular0;
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

uniform PointLight pointLights[4];
uniform DirectLight directLight;

uniform samplerCube environment;
uniform Textures texes;

vec2 parallaxFixedUV(sampler2D parallaxTex, vec3 view);
int chooseCSMLevel(DirectLight light, float depth);
float pointLightShadow(samplerCube depthTex, vec3 fPos, vec3 lightPos, vec3 norm, float zFar);
float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm);


void main() {
    vec3 viewVec = normalize(-fPos);
    vec2 fixedUV = parallaxFixedUV(texes.parallax0, viewVec_tanSpace);

    vec4 texDiff = texture(texes.diffuse0,  fixedUV);
    if (texDiff.a < 0.05) {
        discard;
    }
    texDiff = vec4(pow(texDiff.rgb, vec3(2.2)), texDiff.a);     // Transfer to Linear Space

    vec4 texSpec = texture(texes.specular0, fixedUV);
    vec4 texRfle = texture(texes.reflect0,  fixedUV);
    vec3 texNorm = texture(texes.normals0,  fixedUV).rgb;
    texNorm = normalize(texNorm * 2.0 - 1.0);  // IMPORTANT!!! 需要把坐标从[0, 1]映射到[-1, +1]
    texNorm = TBN * texNorm;


    // ambient
    vec3 ambient = directLight.ambient * texDiff.rgb;


    // directional light
    vec3 dLiInj = mat3(view) * directLight.injection;
    vec3 diffuse_dLight = directLight.color * max(0.0f, dot(-dLiInj, texNorm));

    int i = chooseCSMLevel(directLight, gl_FragCoord.z);
    float dShadow       = directLightShadow(directLight.csmMaps[i], fPosLiSpace[i].xyzw, dLiInj, texNorm);
    diffuse_dLight     *= (1 - dShadow);
    vec3 diffuse        = diffuse_dLight;

    // specular: Blinn-Phong
    float shin          = max(7.82e-3, texes.shininess);     // 0.00782 * 128 ~= 1
    vec3 halfway_dLight = normalize(viewVec - dLiInj);
    vec3 spec_dLight    = pow(max(dot(texNorm, halfway_dLight), 0.0f), shin * 128) * directLight.color;
    spec_dLight        *= (1 - dShadow);
    vec3 specular       = spec_dLight;


    // point light
    for (int i = 0; i < 4; ++i) {
        vec3 LiPos = (view * vec4(pointLights[i].pos, 1.0)).xyz;
        float pShadow       = pointLightShadow(pointLights[i].depthTex, fPos, LiPos, texNorm, pointLights[i].zNearFar.y);
        vec3 inj_pLight     = fPos - LiPos;
        float lightDis      = length(inj_pLight);
        float Kc = pointLights[i].attenu.x;
        float Kl = pointLights[i].attenu.y;
        float Kq = pointLights[i].attenu.z;
        float attenuation   = 1.0 / (Kc + Kl*lightDis + Kq*lightDis*lightDis);   // at linear space
        vec3 pLightResult   = pointLights[i].color * attenuation;
        inj_pLight          = normalize(inj_pLight);
        vec3 diffuse_pLight = pLightResult * max(0.0f, dot(-inj_pLight, texNorm));
        diffuse_pLight     *= (1 - pShadow);
        diffuse += diffuse_pLight;

        // specular
        vec3 halfway_pLight = normalize(-inj_pLight + viewVec);
        vec3 spec_pLight    = pow(max(dot(texNorm, halfway_pLight), 0.0f), shin * 128) * pLightResult;
        spec_pLight        *= (1 - pShadow);
        specular += spec_pLight;
    }

    specular *= texSpec.rgb;
    diffuse  *= texDiff.rgb;


    // reflection
    vec3 refl = reflect(-viewVec, texNorm);
    vec3 reflection = texture(environment, TBN * refl).rgb * texRfle.rgb;

    fragColor = vec4(ambient + diffuse + specular + reflection, 1);


    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);
}


vec2 parallaxFixedUV(sampler2D parallaxTex, vec3 viewVec) {
    float minLayers = 8;
    float maxLayers = 32;

    float nLayers = mix(minLayers, maxLayers, abs(dot(vec3(0, 0, 1), viewVec)));
    float layerDepth = 1 / nLayers;
    vec2 dUV = -viewVec.xy / (nLayers * viewVec.z) * 0.1;

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

int chooseCSMLevel(DirectLight light, float depth) {
    float zNear = 0.1f;
    float zFar  = 100.0f;
    depth = depth * 2.0 - 1.0; // back to NDC
    depth = (2.0 * zNear * zFar) / (zFar + zNear - depth * (zFar - zNear));

    int n = min(MAX_CSM_LEVELS, light.csmLevels);
    for (int i = 0; i < n; ++i) {
        if (depth < light.farDepths[i]) {
            return i;
        }
    }
    return n - 1;
}


vec3 cubeSampleOffsets[20] = {
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
};

float pointLightShadow(samplerCube depthTex, vec3 fPos, vec3 lightPos, vec3 norm, float zFar) {
    vec3 inj_ViSpace = fPos - lightPos;
    vec3 inj_WoSpace = transpose(mat3(view)) * inj_ViSpace;
    float currDepth = length(inj_ViSpace);

    // PCF
    float bias = max(0.1 * (1.0 - dot(norm, -normalize(inj_ViSpace))), 0.002);
    float shadow = 0;
    for (int i = 0; i < 20; ++i) {
        float depth = texture(depthTex, inj_WoSpace + cubeSampleOffsets[i] * 0.005).r;
        depth *= zFar;
        shadow += (currDepth - bias) > depth ? 1.0 : 0.0;
    }
    return shadow / 20;
}


float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm) {
    vec3 projCoords = fPosLSpace.xyz / fPosLSpace.w;
    float currDepth = projCoords.z;
    float bias = max(0.0001 * (1.0 - dot(norm, -injction)), 0.00001);
    vec2 texSize = textureSize(depthTex, 0);

    float shadow = 0;

    // PCF
    int offset = 3;
    float dx = 1.0 / texSize.x;
    float dy = 1.0 / texSize.y;
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