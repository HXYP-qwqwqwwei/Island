#version 460 core
// in 表示输入，location = 0表示位置信息是第0号（也就是第一个）属性
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexUV;
layout (location = 3) in vec3 inNormal;

out vec4 vColor;
out vec3 vNormal;
out vec2 vTexUV;
out vec3 fragPos;

//uniform mat4 transform;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    fragPos = vec3(model * vec4(pos, 1.0));
    vColor = vec4(inColor, 1.0);
    // 对法向量的变换要除去模型矩阵的位移部分，即只取左上角3x3
    vNormal = mat3(model) * inNormal;
    vTexUV = inTexUV;
}