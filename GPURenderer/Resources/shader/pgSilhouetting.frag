#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 MV_Normal;
in vec3 MV_FragPos;
in vec3 MV_LightPos;

in float NdotL;

flat in int Pass;

uniform sampler2D texture_diffuse1;
uniform vec3 color;

vec3 SurfaceColor = vec3(0.75, 0.75, 0.75);
vec3 WarmColor = vec3(0.1, 0.4, 0.8);
vec3 CoolColor = vec3(0.6, 0.0, 0.0);
float DiffuseWarm = 0.45;
float DiffuseCool = 0.045;

vec3 BaseColor = vec3(0.75);
//Gooch
void main(){
    if(Pass == 1)
    {
        FragColor = vec4(vec3(0.2),1.0);
    }
    else if(Pass == 2)
    {
        FragColor = vec4(vec3(0.9),1.0);
    }
}
