#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aUV;
layout (location = 4) in ivec4 boneIds;
layout (location = 5) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec3 Tangent;
out vec2 UV;

out vec3 TangentFragPos;
out vec3 TangentViewPos;

out vec4 FragPosLightSpace;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    vec3 totalTangent = vec3(0.0f);
    
    bool hasBones = false; 

    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
            
        hasBones = true;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * weights[i];
        
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
        totalNormal += localNormal * weights[i];

        vec3 localTangent = mat3(finalBonesMatrices[boneIds[i]]) * aTangent;
        totalTangent += localTangent * weights[i];
    }
    
    if(!hasBones) {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal = aNormal;
        totalTangent = aTangent;
    }

    gl_Position = projection * view * model * totalPosition;
    
    FragPos = vec3(model * totalPosition);
    Normal  = mat3(transpose(inverse(model))) * totalNormal;
    Tangent = mat3(transpose(inverse(model))) * totalTangent;
    UV = aUV;


    // Parallax mapping
    vec3 T = normalize(mat3(model) * totalTangent); 
    vec3 N = normalize(mat3(model) * totalNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    TangentFragPos = TBN * vec3(model * totalPosition); 
    TangentViewPos = TBN * viewPos;

    // Shadows
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}