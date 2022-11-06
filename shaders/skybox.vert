#version 460
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 ViewProjection;

void main()
{
    TexCoords = aPos;
    vec4 pos = ViewProjection * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}