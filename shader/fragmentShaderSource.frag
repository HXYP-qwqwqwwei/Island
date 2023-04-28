#version 460 core
in vec4 vColor;
in vec3 vNormal;
in vec2 vTexUV;
in vec3 fragPos;

out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D specularMap;
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

//uniform vec3 light;
//uniform vec3 lightPos;
uniform vec3 viewPos;

struct Material {
    vec3 specular;
    vec3 diffuse;
    vec3 ambient;
    float shininess;
};

uniform Material material;
uniform PointLightSrc pointLight;
uniform SpotLight spotLight;

void main() {

    // 贴图
    vec4    texColor    = texture(tex, vTexUV);
    texColor *= vColor;

    // 环境光
//    vec3    ambient     = material.ambient * texColor.rgb * light;
    vec3    ambient     = material.ambient * texColor.rgb * pointLight.color;

    // 点光源衰减
    float   dis         = distance(fragPos, pointLight.pos);
    float   attenuation = 1.0f / (1.0f + pointLight.linear*dis + pointLight.quadratic*dis*dis);
    vec3    light       = pointLight.color * attenuation;

    // 点光源
    vec3    norm        = normalize(vNormal);
    vec3    injection   = normalize(fragPos - pointLight.pos);
    float diffStrength  = max(dot(norm, -injection), 0.0f);
    vec3  diffuseLight  = diffStrength * light;
    // 聚光（手电筒）
//    injection = normalize(fragPos - spotLight.pos);
//    float cosTheta = dot(normalize(spotLight.direction), -injection);
//    diffStrength = (cosTheta - cos(radians(15.0f))) / (cos(radians(10.0f)) - cos(radians(15.0f)));
//    diffStrength = clamp(diffStrength, 0.0f, 1.0f);
//    dis = distance(fragPos, spotLight.pos);
//    attenuation = 1.0f / (1.0f + 0.01*dis*dis);
//    diffuseLight += spotLight.color * attenuation * diffStrength;
    // 合并散射光
    vec3 diffuse = diffuseLight * material.diffuse * texColor.rgb;

    // 镜面反射光
    // 如果shininess为0，在pow计算幂的时候，可能会产生不可预知的结果，进而影响其他光照分量
    float   shin        = max(7.82e-3, material.shininess);     // 0.00782 * 128 ~= 1
    vec3    viewVec     = normalize(viewPos - fragPos);
    vec3    reflection  = reflect(injection, norm);
    float   spec        = pow(max(dot(reflection, viewVec), 0.0f), shin * 128);
    vec4    specMap     = texture(specularMap, vTexUV);
    vec3    specular    = (spec * material.specular * specMap.rgb) * light;

    fragColor = vec4(ambient + diffuse + specular, 1);
}
