#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform vec2 iResolution;

void main()
{ 
	vec2 q = gl_FragCoord.xy / iResolution.xy;

    vec3 rgb = texture(screenTexture, TexCoords).xyz;
    
	// vigneting
    rgb *= 0.5 + 0.5*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.15 );

    FragColor = vec4(rgb, 1.0);
}