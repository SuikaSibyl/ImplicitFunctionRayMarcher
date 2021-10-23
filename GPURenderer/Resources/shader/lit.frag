#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 MV_Normal;
in vec3 MV_FragPos;
in vec3 MV_LightPos;

uniform sampler2D texture_diffuse1;
uniform vec3 color;

void main()
{
    float specularStrength = 0.5;
    vec3 objectColor = vec3(1.0, 0.5, 0.31);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Distance
    vec3 light_frag_distance = vec3(length(MV_LightPos - MV_FragPos)*0.05);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(MV_Normal);
    vec3 lightDir = vec3(normalize(MV_LightPos - MV_FragPos));

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize( - MV_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Res
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
    
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}