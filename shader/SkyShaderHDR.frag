#version 460 core
in vec3 fTexUVW;

uniform samplerCube texture0;
uniform float lod = 0.0;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {
    vec3 color = textureLod(texture0, fTexUVW, lod).rgb;
    fragColor = vec4(color, 1.0);

    brightColor = vec4(0.0);
}