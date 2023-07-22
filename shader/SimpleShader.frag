#version 460 core
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform vec3 color;

void main() {
    fragColor = vec4(color, 1);

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        brightColor = fragColor;
    } else brightColor = vec4(0, 0, 0, 1.0);

}