#version 460 core
in vec2 fTexUV;

uniform sampler2D texture0;
uniform sampler2D texture1;
out vec4 fragColor;


void main() {
    vec3 colorHDR = texture(texture0, fTexUV).rgb;
    vec3 bloom = texture(texture1, fTexUV).rgb;
    colorHDR += bloom;
    float gamma = 2.2;
    // Reinhard
    vec3 mapped = colorHDR / (colorHDR + vec3(1.0));

    // Exposure
//    vec3 mapped = vec3(1.0) - exp(-colorHDR * 1.0);
    fragColor = vec4(pow(mapped, vec3(1.0/gamma)), 1);
}
