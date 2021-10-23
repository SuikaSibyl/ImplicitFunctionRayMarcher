#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
	gl_FragColor = vec4(0.8,0.4,0.8,1.0);
}