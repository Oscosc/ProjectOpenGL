#version 330 core
layout (location = 0) in vec3 aPos;
out vec2 fragUV;

void main()
{
    fragUV = (aPos.xy + 1.0) * 0.5;
    gl_Position = vec4(aPos, 1.0);
}