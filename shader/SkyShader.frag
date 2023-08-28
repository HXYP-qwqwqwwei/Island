#version 460 core
in vec3 fTexUVW;

uniform samplerCube texture0;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {
    float gamma = 2.2;
    vec3 color = texture(texture0, fTexUVW).rgb;
    color = pow(color, vec3(gamma));
    // inv-Reinhard, LDR -> HDR
    color = color / (1.0 - color + 0.001);
    fragColor = vec4(color, 1.0);

    brightColor = vec4(0.0);
}