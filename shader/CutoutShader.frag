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
    float shininess;
};

uniform PointLightSrc pointLight;
uniform SpotLight spotLight;
uniform DirectLight directLight;
uniform Material material;
uniform vec3 viewPos;


void main() {
    vec4 texDiff = texture(material.texture_diffuse0, fTexUV);
    if (texDiff.a == 0) {
        discard;
    }
    vec4 texSpec = texture(material.texture_specular0, fTexUV);
    vec4 texNorm = texture(material.texture_normals0, fTexUV);
    // ambient
    vec3 ambient = directLight.ambient * texDiff.rgb;

    // directional light
    vec3 inj_dLight     = normalize(directLight.injection);
    vec3 diffuse_dLight = directLight.color * texDiff.rgb * max(0.0f, dot(-inj_dLight, fNormal));
    // TODO sopt light
    // point light
    float lightDis      = distance(pointLight.pos, fPos);
    float attemuation   = 1.0 / lightDis;
    vec3 plightResult   = pointLight.color * attemuation;
    vec3 inj_pLight     = normalize(fPos - pointLight.pos);
    vec3 diffuse_pLight = plightResult * texDiff.rgb * max(0.0f, dot(-inj_pLight, fNormal));
    // diffuse
    vec3 diffuse = diffuse_dLight + diffuse_pLight;

    // specular
    float shin          = max(7.82e-3, material.shininess);     // 0.00782 * 128 ~= 1
    vec3 ref_pLight     = reflect(inj_pLight, fNormal);
    vec3 ref_dLight     = reflect(inj_dLight, fNormal);
    vec3 view           = normalize(viewPos - fPos);
    vec3 spec_pLight    = pow(max(dot(view, ref_pLight), 0.0f), shin * 128) * plightResult;
    vec3 spec_dLight    = pow(max(dot(view, ref_dLight), 0.0f), shin * 128) * directLight.color;
    vec3 specular       = (spec_pLight + spec_dLight) * texSpec.rgb;

    fragColor = vec4(ambient + diffuse + specular, 1);
}