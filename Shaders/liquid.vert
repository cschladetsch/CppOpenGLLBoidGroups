#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aRadius;

out vec3 FragColor;
out vec2 TexCoord;
out float Radius;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aRadius * 2.0;
    FragColor = aColor;
    Radius = aRadius;
}