#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 MV_Normal;
in vec3 MV_FragPos;
in vec3 MV_LightPos;

uniform sampler2D texture_diffuse1;
uniform vec3 color;

void main(){
	vec3 normal = MV_Normal;
	vec3 lightVec = vec3(normalize(MV_LightPos - MV_FragPos));
	vec3 viewVec = -MV_FragPos;

	vec3 norm = normalize(normal);
	vec3 L = normalize(lightVec);
	vec3 V = normalize(viewVec);

	vec3 halfAngle = normalize(L + V);
	float NdotL = dot(L, norm);
	float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);

    // "Half-Lambert" technique for more pleasing diffuse term
	float diffuse  = 0.5 * NdotL + 0.5;
	float specular = pow(NdotH, 64.0);

	float result = diffuse + specular;
	gl_FragColor = vec4(result);
}
