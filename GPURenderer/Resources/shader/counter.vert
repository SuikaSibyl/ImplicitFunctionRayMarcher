#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aNormal_1;
layout (location = 3) in vec3 aNormal_2;

out vec3 MV_Normal;
out vec3 MV_FragPos;
out vec3 MV_LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

uniform vec3 lightPos;
uniform float thickness;

void main()
{
    mat4 MV_mat = view * model;
    mat4 MVP_mat = projection * MV_mat;
    
    // Light
    // -----
    MV_LightPos = vec3(view * vec4(lightPos, 1.0));
    MV_FragPos = vec3(view * model * vec4(aPos, 1.0));
    MV_Normal = normal * aNormal;
    vec3 MV_Normal_1 = normal * aNormal_1;
    vec3 MV_Normal_2 = normal * aNormal_2;

    if(aNormal_1.x==0 && aNormal_1.y==0 && aNormal_1.z==0)
        gl_Position = MVP_mat * vec4(aPos, 1.0);
    else
    {
        if(dot(MV_FragPos,MV_Normal_1)>0.0f || dot(MV_FragPos,MV_Normal_2)> 0.0f)
            gl_Position = MVP_mat * vec4(aPos+ thickness * aNormal, 1.0);
        else
            gl_Position = MVP_mat * vec4(aPos, 1.0);
    }
}