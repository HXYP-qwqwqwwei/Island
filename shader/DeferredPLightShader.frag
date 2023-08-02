#version 460 core

struct Textures {
    sampler2D diffuse0; // pos
    sampler2D diffuse1; // norm
    sampler2D diffuse2; // diff
    sampler2D diffuse3; // spec
};


layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct PointLight {
    vec3 pos;
    vec3 color;
    vec3 attenu;
    vec2 zNearFar;
    samplerCube depthTex;
};

uniform PointLight pointLight;
uniform Textures texes;

uniform vec3 viewPos;

float pointLightShadow(samplerCube depthTex, vec3 fPos, vec3 lightPos, vec3 norm, float zFar);


void main() {
    float gamma = 2.2;
    vec2 uv = gl_FragCoord.xy / textureSize(texes.diffuse0, 0);
    vec3 fPos    = texture(texes.diffuse0, uv).xyz;
    vec3 texNorm = texture(texes.diffuse1, uv).xyz;

    vec3 texDiff = texture(texes.diffuse2, uv).rgb;
    texDiff = pow(texDiff, vec3(gamma));

    vec3 texSpec = texture(texes.diffuse3, uv).rgb;

    vec3 view = normalize(viewPos - fPos);

    float shin = max(7.82e-3, 2.0);     // 0.00782 * 128 ~= 1

    float pShadow = pointLightShadow(pointLight.depthTex, fPos, pointLight.pos, texNorm, pointLight.zNearFar.y);
    vec3 inj_pLight = fPos - pointLight.pos;
    float dis = length(inj_pLight);
    float Kc = pointLight.attenu.x;
    float Kl = pointLight.attenu.y;
    float Kq = pointLight.attenu.z;
    float attenuation   = 1.0 / (Kc + Kl*dis + Kq*dis*dis);   // at linear space
    inj_pLight /= dis;
    vec3 diff_pLight = pointLight.color * (max(dot(-inj_pLight, texNorm), 0.0) * attenuation);
    diff_pLight *= (1.0 - pShadow);

    vec3 halfway_pLight = normalize(view - inj_pLight);
    vec3 spec_pLight = pointLight.color * (pow(max(dot(halfway_pLight, texNorm), 0.0), shin * 128) * attenuation);
    spec_pLight *= (1.0 - pShadow);

    vec3 diffuse = diff_pLight * texDiff.rgb;
    vec3 specular = spec_pLight * texSpec.rgb;

    vec3 color = diffuse + specular;
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