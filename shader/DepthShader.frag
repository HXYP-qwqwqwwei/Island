#version 460 core

//out vec4 fragColor;
in vec2 fTexUV;


struct Textures {
    sampler2D diffuse0;
};

uniform Textures texes;

void main() {
    vec4 texDiff = texture(texes.diffuse0, fTexUV);
    if (texDiff.a == 0) {
        discard;
    }
//    fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}