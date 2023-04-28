#version 460 core
in vec2 fTexUV;

uniform sampler2D texture0;

out vec4 fragColor;

const float offset = 1.0 / 900.0;

vec2 offsets[9] = {
    vec2(-offset, offset),  // LR
    vec2(0.0f   , offset),  // T
    vec2(offset , offset),  // RR
    vec2(-offset, 0.0f),    // L
    vec2(0.0f   , 0.0f),    // C
    vec2(offset , 0.0f),    // R
    vec2(-offset, -offset), // LL
    vec2(0.0f   , -offset), // D
    vec2(offset , -offset)  // RL
};

vec3 convolution(sampler2D tex, vec2 uv, float core[9]);


void main() {
    vec3 color = texture(texture0, fTexUV).rgb;
    // reverse
//    fragColor = vec4(1-color, 1);
    // binary
//    float avg = (0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b);
//    fragColor = vec4(avg, avg, avg, 1);
    // 2D-Convolution
    float shapen[9] = {
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
    };

    float blur[9] = {
            1.0/16, 2.0/16, 1.0/16,
            2.0/16, 4.0/16, 2.0/16,
            1.0/16, 2.0/16, 1.0/16
    };

    float edge[9] = {
            1, 1, 1,
            1,-9, 1,
            1, 1, 1
    };

//    fragColor = vec4(convolution(texture0, fTexUV, shapen), 1);

    // Gamma-correction
    float gamma = 2.2;
    fragColor = vec4(pow(color, vec3(1/gamma)), 1);
}

vec3 convolution(sampler2D tex, vec2 uv, float kernel[9]) {
    vec3 color = vec3(0);
    for (int i = 0; i < 9; ++i) {
        vec3 texColor = texture(tex, uv + offsets[i]).rgb;
        color += texColor * kernel[i];
    }
    return color;
}