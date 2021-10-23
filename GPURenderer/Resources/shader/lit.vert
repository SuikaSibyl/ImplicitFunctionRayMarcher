#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out vec3 MV_Normal;
out vec3 MV_FragPos;
out vec3 MV_LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

uniform vec3 lightPos;

void main()
{
    mat4 MV_mat = view * model;
    mat4 MVP_mat = projection * MV_mat;
    
    // Light
    // -----
    MV_LightPos = vec3(view * vec4(lightPos, 1.0));
    MV_FragPos = vec3(view * model * vec4(aPos, 1.0));
    MV_Normal = normal * aNormal;

    TexCoords = aTexCoords;

    gl_Position = MVP_mat * vec4(aPos, 1.0);
}