#version 460 core

in vec2 fTexUV;
out vec4 fragColor;

uniform sampler2D texture0;
uniform bool horizontal;

float gaussianCore[5] = {0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216};

void main() {
    vec2 offset = 1.0 / textureSize(texture0, 0);
    vec3 result = texture(texture0, fTexUV).rgb * gaussianCore[0];
    if (horizontal) {
        for (int i = 1; i < 5; ++i) {
            result += texture(texture0, fTexUV + vec2(offset.x * i, 0)).rgb * gaussianCore[i];
            result += texture(texture0, fTexUV - vec2(offset.x * i, 0)).rgb * gaussianCore[i];
        }
    } else {
        for (int i = 1; i < 5; ++i) {
            result += texture(texture0, fTexUV + vec2(0, offset.y * i)).rgb * gaussianCore[i];
            result += texture(texture0, fTexUV - vec2(0, offset.y * i)).rgb * gaussianCore[i];
        }
    }
    fragColor = vec4(result, 1.0);
}