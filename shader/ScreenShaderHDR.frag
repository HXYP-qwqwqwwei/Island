#version 460 core
in vec2 fTexUV;

uniform sampler2D texture0;
out vec4 fragColor;


void main() {
    vec3 colorHDR = texture(texture0, fTexUV).rgb;
    float gamma = 2.2;
    // Reinhard
//    vec3 mapped = colorHDR / (colorHDR + vec3(1.0));

    // exposure
    vec3 mapped = vec3(1.0) - exp(-colorHDR * 1.0);
//    mapped = colorHDR;
    fragColor = vec4(pow(mapped, vec3(1/gamma)), 1);

//    fragColor = vec4(mapped, 1);

}
