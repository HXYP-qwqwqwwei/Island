#version 460 core
in vec3 fTexUVW;

uniform samplerCube texture0;

out vec4 fragColor;

void main() {
    fragColor = texture(texture0, fTexUVW);
}