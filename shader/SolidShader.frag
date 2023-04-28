#version 460 core
in vec3 fPos;
in vec3 fNormal;
in vec2 fTexUV;

out vec4 fragColor;

struct PointLightSrc {
    vec3 color;
    vec3 pos;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 color;
    vec3 pos;
    vec3 direction;
    float linear;
    float quadratic;
    float cutOff;
};

struct DirectLight {
    vec3 injection;
    vec3 color;
    vec3 ambient;
};

#define N_TEXTURE 4
struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    sampler2D texture_normals0;
    sampler2D texture_reflect0;
    float shininess;
};

uniform samplerCube environment;
uniform PointLightSrc pointLight;
uniform SpotLight spotLight;
uniform DirectLight directLight;
uniform Material material;
uniform vec3 viewPos;


void main() {
    vec4 texDiff = texture(material.texture_diffuse0, fTexUV);
    vec4 texSpec = texture(material.texture_specular0, fTexUV);
    vec4 texNorm = texture(material.texture_normals0, fTexUV);
    vec4 texRfle = texture(material.texture_reflect0, fTexUV);
    // ambient
    vec3 ambient = directLight.ambient * texDiff.rgb;

    // directional light
    vec3 inj_dLight     = normalize(directLight.injection);
    vec3 diffuse_dLight = directLight.color * texDiff.rgb * max(0.0f, dot(-inj_dLight, fNormal));
    // TODO sopt light
    // point light
    float lightDis      = distance(pointLight.pos, fPos);

    // at non-linear space
//    float attemuation   = 1.0 / (1.0 + lightDis * pointLight.linear + lightDis * lightDis * pointLight.quadratic);

    float attemuation   = 1.0 / lightDis;   // at linear space
    vec3 plightResult   = pointLight.color * attemuation;
    vec3 inj_pLight     = normalize(fPos - pointLight.pos);
    vec3 diffuse_pLight = plightResult * texDiff.rgb * max(0.0f, dot(-inj_pLight, fNormal));
    // diffuse
    vec3 diffuse = diffuse_dLight + diffuse_pLight;

    // specular
    float shin          = max(7.82e-3, material.shininess);     // 0.00782 * 128 ~= 1
    vec3 view           = normalize(viewPos - fPos);
    // Phong
//    vec3 ref_pLight     = reflect(inj_pLight, fNormal);
//    vec3 ref_dLight     = reflect(inj_dLight, fNormal);
//    vec3 spec_pLight    = pow(max(dot(view, ref_pLight), 0.0f), shin * 128) * plightResult;
//    vec3 spec_dLight    = pow(max(dot(view, ref_dLight), 0.0f), shin * 128) * directLight.color;
    // Blinn-Phong
    vec3 halfway_pLight = normalize(-inj_pLight + view);
    vec3 halfway_dLight = normalize(-inj_dLight + view);
    vec3 spec_pLight    = pow(max(dot(fNormal, halfway_pLight), 0.0f), shin * 128) * plightResult;
    vec3 spec_dLight    = pow(max(dot(fNormal, halfway_dLight), 0.0f), shin * 128) * directLight.color;

    vec3 specular       = (spec_pLight + spec_dLight) * texSpec.rgb;

    // reflection
    vec3 rayTrace       = -view;
    vec3 reflectionVec  = reflect(rayTrace, fNormal);
    vec3 reflection     = texture(environment, reflectionVec).rgb * texRfle.rgb;

    fragColor = vec4(ambient + diffuse + specular + reflection, 1);

//    fragColor = vec4(reflection, 1);
}