#version 460 core
in vec2 fTexUV;
in mat4 view_inv;
in vec3 dLightInj_viewSpace;

uniform sampler2D texture0; // pos & depth
uniform sampler2D texture1; // norm
uniform sampler2D texture2; // diff
uniform sampler2D texture3; // spec
uniform sampler2D texture4; // ssao

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct DirectLight {
    vec3 injection;
    vec3 color;
    vec3 ambient;
    sampler2D depthTex;
};

uniform DirectLight directLight;

uniform mat4 lightSpaceMtx;

float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm);


void main() {
    float gamma = 2.2;
    vec3 fPos    = texture(texture0, fTexUV).xyz;
    vec3 texNorm = texture(texture1, fTexUV).xyz;

    vec3 texDiff = texture(texture2, fTexUV).rgb;
    texDiff = pow(texDiff, vec3(gamma));

    vec3 texSpec = texture(texture3, fTexUV).rgb;
    float occlusion = texture(texture4, fTexUV).r;

    float dShadow = directLightShadow(directLight.depthTex, lightSpaceMtx * view_inv * vec4(fPos, 1.0), directLight.injection, texNorm);

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

    vec3 specular = spec_dLight * texSpec;
    vec3 diffuse  = diff_dLight * texDiff;

    vec3 color = ambient + diffuse + specular;
    fragColor = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);

}


float directLightShadow(sampler2D depthTex, vec4 fPosLSpace, vec3 injction, vec3 norm) {
    vec3 projCoords = fPosLSpace.xyz / fPosLSpace.w;
    // 将坐标范围从[-1, 1]映射到[0, 1]
    projCoords = (projCoords + 1.0) * 0.5;
    float currDepth = projCoords.z;
    float bias = max(0.0001 * (1.0 - dot(mat3(view_inv) * norm, -normalize(injction))), 0.00001);
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