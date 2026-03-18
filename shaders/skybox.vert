#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoords = aPos;   // use cube vertex position as cubemap lookup direction
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // trick: set z = w so depth is always 1.0 (farthest)
}
