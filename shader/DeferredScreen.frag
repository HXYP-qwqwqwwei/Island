#version 460 core
in vec2 fTexUV;

uniform sampler2D texture0; // pos
uniform sampler2D texture1; // norm
uniform sampler2D texture2; // diff
uniform sampler2D texture3; // spec'

out vec4 fragColor;

struct PointLight {
    vec3 pos;
    vec3 color;
    float linear;
    float zFar;
    samplerCube shadowMap;
};

struct DirectLight {
    vec3 color;
    vec3 ambient;
    sampler2D shadowMap;
};

uniform PointLight pointLights[4];
uniform DirectLight directLight;


void main() {
    vec3 fPos    = texture(texture0, fTexUV).xyz;
    vec3 texNorm = texture(texture1, fTexUV).xyz;
    vec3 texDiff = texture(texture2, fTexUV).rgb;
    vec3 texSpec = texture(texture3, fTexUV).rgb;

    fragColor = vec4(texNorm, 1);
}