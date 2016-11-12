#version 150 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;
in vec2 Normaltexcoord;

out vec4 outColor;

uniform sampler2D texKitten;
uniform sampler2D texPuppy;
uniform sampler2D texNormalMap;
uniform float time;

uniform vec3 cameraPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light lights[10];  


void main() {
	Light light = lights[0];
   //outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), time);
	vec3 objectColor = {1,1,1};
	//Ambient
	float ambientStrength = 0.25f;
	//Specular
	float specularStrength = 0.5f;
	
	vec3 ambient = light.ambient * material.ambient;

	//Uncomment one of the next two lines
	vec3 norm = normalize(Normal); //Per vertex normals
	//vec3 norm = normalize(texture(texNormalMap, Normaltexcoord).rgb); //Normal maps
	
	vec3 lightDir = normalize(light.position - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	//Calculate specular lighting
	//I don't really understand that math behind this
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec + material.specular);


	ambient  = light.ambient * material.ambient;
	diffuse  = light.diffuse * (diff * material.diffuse);
	specular = light.specular * (spec * material.specular);
	
	vec3 result = (ambient + diffuse + specular) * objectColor;

	outColor = vec4(result, 1.0f);
}