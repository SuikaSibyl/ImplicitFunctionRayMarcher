#version 330 core
out vec4 FragColor;

in vec3 MV_Normal;
in vec3 MV_FragPos;
in vec3 MV_LightPos;

void main()
{
    FragColor = vec4(vec3(1), 1.0);
}