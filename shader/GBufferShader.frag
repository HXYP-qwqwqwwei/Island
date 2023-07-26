#version 460 core

in vec3 fPos;
in vec3 fNormal;
in vec2 fTexUV;
in mat3 TBN;

layout (location = 0) out vec4 gPos;
layout (location = 1) out vec4 gNorm;
layout (location = 2) out vec4 gDiff;
layout (location = 3) out vec4 gSpec;

struct Textures {
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normals0;
    sampler2D reflect0;
    sampler2D parallax0;
    float shininess;
};

uniform Textures texes;


void main() {
    gPos  = vec4(fPos, 1.0);
    vec3 norm = texture(texes.normals0, fTexUV).rgb;
    norm  = 2.0 * norm - 1.0;
    gNorm = vec4(TBN * norm, 1.0);
    gDiff = vec4(texture(texes.diffuse0, fTexUV).rgb, 1.0);
    gSpec = vec4(texture(texes.specular0, fTexUV).rgb, 1.0);
}