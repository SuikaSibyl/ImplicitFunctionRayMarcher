// minimal fragment shader
// www.lighthouse3d.com


void main1()
{
	gl_FragColor = vec4(0.8,0.4,0.8,1.0);
	//gl_FragColor = vec4(1,0,0,1.0);
}

varying vec4 diffuseColor;
varying vec3 lightVector;
varying vec3 fragNormal;
varying vec3 viewVector;

void main2(){

	float lightingFactor =max(dot(lightVector,fragNormal),0.0);

	gl_FragColor = diffuseColor * lightingFactor;

}

//ivory
void main3(){
	vec3 normal = fragNormal;
	vec3 lightVec =lightVector;
	vec3 viewVec = viewVector;

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

//Gooch
void main(){
	vec3 ambient = vec3(0.0, 0.0, 0.0);
	vec3 normal = fragNormal;
	vec3 lightVec =lightVector;
	vec3 viewVec = viewVector;

	const float b = 0.55;
	const float y = 0.3;
	const float Ka = 1.0;
	const float Kd = 0.8;
	const float Ks = 0.9;
	vec3 specularcolor = vec3(1.0, 1.0, 1.0);

	vec3 norm = normalize(normal);
	vec3 L = normalize (lightVec);
	vec3 V = normalize (viewVec);
	vec3 halfAngle = normalize (L + V);

	vec3 orange = vec3(.88,.81,.49);
	vec3 purple = vec3(.58,.10,.76);

	vec3 kCool = purple;
	vec3 kWarm = orange;

	float NdotL = dot(L, norm);
	float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);
	float specular = pow(NdotH, 64.0);

	float blendval  = 0.5 * NdotL + 0.5;
	vec3 Cgooch = mix(kWarm, kCool, blendval);

	vec3 result = Ka * ambient + Kd * Cgooch + specularcolor * Ks * specular;
	gl_FragColor = vec4(result, 1.0);
}

