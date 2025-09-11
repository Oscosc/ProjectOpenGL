#version 460 core

layout (location = 0) in vec2 aPos;   // positions NDC: [-1, +1]

out vec2 TexCoords;   // [0  ;  1]
out vec2 RayUV;       // [-1 ; +1]

void main()
{
    RayUV = aPos;                   // pour le calcul de rayon
    TexCoords = aPos * 0.5 + 0.5;   // NDC -> [0,1] pour sampling
    gl_Position = vec4(aPos, 0.0, 1.0);
}