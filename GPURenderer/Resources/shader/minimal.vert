// minimal vertex shader
// www.lighthouse3d.com



void main1()
{	

	// the following three lines provide the same result

//	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
//	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = ftransform();
}

varying vec4 diffuseColor;
varying vec3 fragNormal;
varying vec3 lightVector;
varying vec3 viewVector;

//uniform vec3 eyeSpaceLightVector;
//vec3 eyeSpaceLightVector = vec3(1.0, 0.0, 0.0);

void main(){
	vec3 eyeSpaceLightVector = gl_LightSource[0].position.xyz;
	vec3 eyeSpaceVertex= vec3(gl_ModelViewMatrix * gl_Vertex);

	lightVector= vec3(normalize(eyeSpaceLightVector - eyeSpaceVertex));
	fragNormal = gl_NormalMatrix * gl_Normal;
	viewVector = -vec3(eyeSpaceVertex);
	
	//diffuseColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
