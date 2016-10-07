#version 150 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
//in vec2 Texcoord;
in vec2 Normaltexcoord;

out vec4 outColor;

//uniform sampler2D texKitten;
//uniform sampler2D texPuppy;
uniform sampler2D texNormalMap;
uniform float time;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main() {
   //outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), time);
	vec3 lightColor = {1,0,1};
	//Ambient
	float ambient = 0.25f;
	//Specular
	float specularStrength = 0.5f;
	
	//Uncomment one of the next two lines
	vec3 norm = normalize(Normal); //Per vertex normals
	//vec3 norm = normalize(texture(texNormalMap, Normaltexcoord).rgb); //Normal maps
	
	vec3 lightDir = normalize(lightPos - FragPos);

	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;


	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (ambient + diff + specular) * lightColor;

	outColor = vec4(diffuse, 1.0f);
}