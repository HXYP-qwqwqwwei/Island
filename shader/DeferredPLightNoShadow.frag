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
};


in PointLight pointLight;

uniform Textures texes;


void main() {
    float gamma = 2.2;
    vec2 uv = gl_FragCoord.xy / textureSize(texes.diffuse0, 0);
    vec3 fPos    = texture(texes.diffuse0, uv).xyz;
    vec3 texNorm = texture(texes.diffuse1, uv).xyz;

    vec3 texDiff = texture(texes.diffuse2, uv).rgb;
    texDiff = pow(texDiff, vec3(gamma));

    vec3 texSpec = texture(texes.diffuse3, uv).rgb;

    // Attenuation
    vec3 inj_pLight = fPos - pointLight.pos;
    float dis = length(inj_pLight);
    float Kc = pointLight.attenu.x;
    float Kl = pointLight.attenu.y;
    float Kq = pointLight.attenu.z;
    float attenuation   = 1.0 / (Kc + Kl*dis + Kq*dis*dis);   // at linear space
    inj_pLight /= dis;

    // Diffuse
    vec3 diff_pLight = pointLight.color * (max(dot(-inj_pLight, texNorm), 0.0) * attenuation);

    // Specular
    vec3 viewVec = normalize(-fPos);
    float shin = max(7.82e-3, 2.0);     // 0.00782 * 128 ~= 1
    vec3 halfway_pLight = normalize(viewVec - inj_pLight);
    vec3 spec_pLight = pointLight.color * (pow(max(dot(halfway_pLight, texNorm), 0.0), shin * 128) * attenuation);

    vec3 diffuse = diff_pLight * texDiff.rgb;
    vec3 specular = spec_pLight * texSpec.rgb;

    vec3 color = diffuse + specular;
    fragColor = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);

}