#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 TexCoords;

    vec3 MV_Normal;
    vec3 MV_FragPos;
    vec3 MV_LightPos;

    vec4 expand;
    int Pass;

    bool backFace;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

uniform vec3 lightPos;

uniform int pass;
uniform float thick;

void main()
{
    mat4 MV_mat = view * model;
    mat4 MVP_mat = projection * MV_mat;
    
    // Light
    // -----
    vs_out.MV_LightPos = vec3(view * vec4(lightPos, 1.0));
    vs_out.MV_FragPos = vec3(view * model * vec4(aPos, 1.0));
    vs_out.MV_Normal = normal * aNormal;

    // MVP_Normal = mat3(transpose(inverse(MVP_mat))) * aNormal;

    vs_out.TexCoords = aTexCoords;
    vs_out.Pass = pass;
    if(pass==1)
    {
        if(dot(vs_out.MV_Normal, (-vs_out.MV_FragPos))<0)
        {
            vs_out.backFace = true;
        }
        else
            vs_out.backFace = false;

        vs_out.expand = MVP_mat * vec4(aPos + aNormal * 0.1, 1.0);
        gl_Position = MVP_mat * vec4(aPos, 1.0);
    }
    else if(pass==2)
    {
        vs_out.backFace = false;
        vs_out.expand = MVP_mat * vec4(aPos + aNormal * 0.1, 1.0);
        gl_Position = MVP_mat * vec4(aPos, 1.0);
    }
}