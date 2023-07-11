#version 460 core

in vec4 fPos;
in vec2 fTexUV;

struct Material {
    sampler2D texture_diffuse0;
};

struct PointLight {
    vec3 pos;
    float zFar;
};

uniform Material material;
uniform PointLight pointLight;


void main() {
    vec4 texDiff = texture(material.texture_diffuse0, fTexUV);
    if (texDiff.a == 0) {
        discard;
    }

    float dis = length(fPos.xyz - pointLight.pos);
    dis /= pointLight.zFar;
    gl_FragDepth = dis;
}