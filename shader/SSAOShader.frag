#version 460 core
in vec2 fTexUV;

out float occlusion;

uniform sampler2D texture0; // pos & depth
uniform sampler2D texture1; // norm
uniform sampler2D texture2; // noise

layout (std140, binding = 0) uniform Matrics {
    mat4 view;
    mat4 proj;
};

#define N_SAMPLES 64
uniform float radius = 1.0;
uniform float power  = 1.0;
uniform vec3 samples[N_SAMPLES];
uniform vec2 screenSize = vec2(1920.0, 1080.0);
uniform vec2 noiseSize = vec2(4.0, 4.0);

void main() {
    vec4 PosDepth = texture(texture0, fTexUV);
    vec3 fPos = PosDepth.xyz;
    vec3 noise = texture(texture2, fTexUV * (screenSize / noiseSize)).xyz;

    vec3 N = texture(texture1, fTexUV).xyz;
    vec3 T = normalize(noise - N * dot(noise, N));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    occlusion = 0.0;
    for (int i = 0; i < N_SAMPLES; ++i) {
        vec3 samp = TBN * samples[i].xyz;   // back to View-Space
        samp = fPos + samp * radius;

        vec4 samp_proj = proj * vec4(samp, 1.0);
        vec2 uv = samp_proj.xy / samp_proj.w;   // [-1.0, +1.0]
        uv = (uv + 1.0) * 0.5;
        float sampDepth = texture(texture0, uv).w;

        // 从边缘出发的采样点可能采样到背后的、距离超出半径的片段，因此使用半径与距离的比值作为权重，来降低边缘的不正确遮蔽
        float weight = smoothstep(0.0, 1.0, radius / abs(fPos.z + sampDepth));
        // 当采样点的深度大于深度贴图对应的深度时，说明该采样点被遮蔽
        occlusion += (-samp.z >= sampDepth  ? 1.0 : 0.0);  // 坐标z值需要取反，是因为在GL观察空间中，z<=0（Z轴正方向在背后）
    }
    occlusion = 1.0 - (occlusion / N_SAMPLES);
    occlusion = pow(occlusion, power);
}