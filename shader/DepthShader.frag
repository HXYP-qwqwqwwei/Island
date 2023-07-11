#version 460 core

//out vec4 fragColor;
in vec2 fTexUV;


struct Material {
    sampler2D texture_diffuse0;
};

uniform Material material;

void main() {
    vec4 texDiff = texture(material.texture_diffuse0, fTexUV);
    if (texDiff.a == 0) {
        discard;
    }
//    fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}