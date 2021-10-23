#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 TexCoords;

    vec3 MV_Normal;
    vec4 MV_FragPos;
    vec3 MV_LightPos;

    mat4 Project;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

uniform vec3 lightPos;

void main()
{
    mat4 MV_mat = view * model;
    mat4 MVP_mat = projection * MV_mat;
    vs_out.Project = projection;
    // Light
    // -----
    vs_out.MV_LightPos = vec3(view * vec4(lightPos, 1.0));
    vs_out.MV_FragPos = view * model * vec4(aPos, 1.0);
    vs_out.MV_Normal = normalize(normal * aNormal);

    vs_out.TexCoords = aTexCoords;

    gl_Position = MVP_mat * vec4(aPos, 1.0);
}