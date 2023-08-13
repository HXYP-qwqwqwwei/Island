#version 460 core
in vec2 fTexUV;
in mat3 view_inv3x3;
in vec3 dLightInj_viewSpace;

uniform sampler2D texture0; // pos & depth
uniform sampler2D texture1; // norm
uniform sampler2D texture2; // diff
uniform sampler2D texture3; // spec
uniform sampler2D texture4; // ssao

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

#define MAX_CSM_LEVELS 4

struct DirectLight {
    vec3 injection;
    vec3 color;
    vec3 ambient;
    sampler2D csmMaps[MAX_CSM_LEVELS];
    mat4 LiSpaceMatrices[MAX_CSM_LEVELS];
    float farDepths[MAX_CSM_LEVELS];
    int csmLevels;
};

uniform DirectLight directLight;

float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm);
int chooseCSMLevel(DirectLight light, float depth);


void main() {
    float gamma = 2.2;
    vec4 PosDepth = texture(texture0, fTexUV);
    vec3 fPos    = PosDepth.xyz;
    vec3 texNorm = texture(texture1, fTexUV).xyz;

    vec3 texDiff = texture(texture2, fTexUV).rgb;
    texDiff = pow(texDiff, vec3(gamma));

    vec3 texSpec = texture(texture3, fTexUV).rgb;
    float occlusion = texture(texture4, fTexUV).r;

    int i = chooseCSMLevel(directLight, PosDepth.w);
    float dShadow = directLightShadow(directLight.csmMaps[i], directLight.LiSpaceMatrices[i] * vec4(fPos, 1.0), directLight.injection, texNorm);

    // Ambient
    vec3 ambient = directLight.ambient * texDiff * occlusion;

    // Diffuse
    vec3 inj_dLight  = normalize(dLightInj_viewSpace);
    vec3 diff_dLight = directLight.color * max(0.0, dot(-inj_dLight, texNorm));
    diff_dLight *= (1.0 - dShadow);

    // Specular
    vec3 viewVec = normalize(-fPos);
    float shin          = max(7.82e-3, 2.0);     // 0.00782 * 128 ~= 1
    vec3 halfway_dLight = normalize(viewVec - inj_dLight);
    vec3 spec_dLight    = directLight.color * pow(max(dot(halfway_dLight, texNorm), 0.0), shin * 128);
    spec_dLight *= (1.0 - dShadow);

    vec3 specular = spec_dLight * texSpec;
    vec3 diffuse  = diff_dLight * texDiff;

    vec3 color = ambient + diffuse + specular;
    fragColor = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);

}

int chooseCSMLevel(DirectLight light, float depth) {
    int n = min(MAX_CSM_LEVELS, light.csmLevels);
    for (int i = 0; i < n; ++i) {
        if (depth < light.farDepths[i]) {
            return i;
        }
    }
    return n - 1;
}


float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm) {
    vec3 projCoords = fPosLSpace.xyz / fPosLSpace.w;
    float currDepth = projCoords.z;
    float bias = max(0.0001 * (1.0 - dot(view_inv3x3 * norm, -normalize(injction))), 0.00001);
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