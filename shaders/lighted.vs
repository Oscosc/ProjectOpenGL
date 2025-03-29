#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aUV;

out vec3 FragPos; // Fragment position
out vec3 Normal; // Normal value
out vec3 UV; // UV Map (only Bezier Curve)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    UV = aUV;
}