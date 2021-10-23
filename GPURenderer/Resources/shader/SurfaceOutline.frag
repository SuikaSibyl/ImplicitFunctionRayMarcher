#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 MV_Normal;
in vec3 MV_FragPos;
in vec3 MV_LightPos;

in float NdotL;

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
	vec3 lightVec = vec3(normalize(MV_LightPos - MV_FragPos));
	vec3 ViewVec = normalize(-MV_FragPos);
	vec3 norm = normalize(MV_Normal);
	vec3 L = normalize (lightVec);
	vec3 V = normalize (ViewVec);
	vec3 halfAngle = normalize (L + V);

	float NdotL = 0.5 * dot(L, norm) + 0.5;
	// float oneMinusDot = dot(vec3(0,0,1), norm);
	float NdotV = dot(ViewVec, norm);

	if(NdotV > 0.3)
		FragColor = vec4(vec3(0.8),1.0);
	else
		FragColor = vec4(vec3(NdotV),1.0);

	// // maybe can * gl_Color
	// vec3 kCool = min(CoolColor + DiffuseCool * BaseColor, 1.0);
	// vec3 kWarm = min(WarmColor + DiffuseWarm * BaseColor, 1.0);
	// vec3 kfinal = mix(kWarm, kCool, NdotL);

	// vec3 ReflectVec = reflect(-lightVec, MV_Normal);

	// float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);
	// float specular = pow(NdotH, 64.0);

	// vec3 nreflect = normalize(ReflectVec);
	// vec3 nview = normalize(ViewVec);
	// float spec = max(dot(nreflect, nview), 0.0);
	// spec = pow(spec, 32.0);

	// FragColor = vec4(min(kfinal + spec, 1.0),1.0);
}

// varying vec3 ReflectVec;
// varying vec3 ViewVec;
// void main() {

// vec3 nreflect = normalize(ReflectVec);
// vec3 nview = normalize(ViewVec);
// float spec = max(dot(nreflect, nview),
// 0.0);
// spec = pow(spec, 32.0);
// gl_FragColor = vec4(min(kfinal + spec, 1.0),
// 1.0);
// }