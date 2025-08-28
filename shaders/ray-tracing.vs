#version 330 core

layout (location = 0) in vec2 aUV;

out vec2 UV;

void main()
{
    UV = aUV;
    gl_Position = vec4(aUV, 0.0, 1.0);
}