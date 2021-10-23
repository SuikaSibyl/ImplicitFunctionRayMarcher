#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

float rgb2gray(vec3 rgb)
{
    return rgb[0]*0.299 + rgb[1]*0.587 + rgb[2]*0.114;
}

void main()
{ 
    vec4 rgba = texture(screenTexture, TexCoords);
    FragColor = vec4(vec3(rgb2gray(rgba.rgb)), 1.0);
}