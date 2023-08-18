#version 460 core
in vec3 fTexUVW;

#define PI 3.1415926538f
#define TWO_PI PI * 2.0

uniform sampler2D texture0;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {
    vec3 nVec = normalize(fTexUVW);

    float beta  = atan(nVec.x, nVec.z);
    float alpha = asin(nVec.y);

    vec2 uv = vec2(beta, alpha) / vec2(TWO_PI, PI);
    uv = uv + 0.5;

    vec3 color = texture(texture0, uv).rgb;
    fragColor = vec4(color, 1.0);

    brightColor = vec4(0.0);
}