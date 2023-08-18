#version 460 core
in vec3 fTexUVW;

#define PI      3.1415926538f
#define TWO_PI  PI * 2.0
#define HALF_PI PI / 2.0

uniform samplerCube texture0;
uniform int nSegments = 1;

out vec3 irradiance;

const vec3 up = vec3(0.0, 1.0, 0.0);

void main() {
    vec3 color = texture(texture0, fTexUVW).rgb;
    irradiance = vec3(0.0);
    int nSegmentsXZ = nSegments * 4;
    float dTheta = HALF_PI / nSegments;
    float dPhi   = TWO_PI / nSegmentsXZ;

    vec3 N = normalize(fTexUVW);
    vec3 T = normalize(cross(up, N));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    for (int i = 0; i < nSegments; ++i) {
        for (int j = 0; j < nSegmentsXZ; ++j) {
            float theta = i * dTheta;
            float phi = j * dPhi;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            vec3 v = TBN * vec3(sinTheta * cos(phi), sinTheta * sin(phi), cos(theta));  // In tangent-space
            vec3 L = texture(texture0, v).rgb;
            irradiance += (L * sinTheta * cosTheta);
        }
    }
    irradiance *= (vec3(PI) / float(nSegments * nSegmentsXZ));
}