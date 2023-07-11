#version 460 core
layout (triangles) in;
// 对于每个输入的三角形，shader需要绘制到六个面上，因此输出顶点数为3*6=18
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 cubeSpaceMatrices[6];     // 6个空间的变换矩阵
in G_IN {
    vec2 gTexUV;
} g_in[];

out vec4 fPos;
out vec2 fTexUV;

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face;    // gl_Layer指定渲染到立方体贴图的哪个面
        for (int i = 0; i < 3; ++i) {   //  对输入三角形的每个顶点
            fPos = gl_in[i].gl_Position;
            fTexUV = g_in[i].gTexUV;
            gl_Position = cubeSpaceMatrices[face] * fPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}