#version 460 core

void main() {

}


float linearizeDepth(float depth, float zNear, float zFar) {
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}
