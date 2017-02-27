#version 150 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texDiffuseMap; //diffuse map
uniform sampler2D texPuppy;
uniform sampler2D texNormalMap;
uniform sampler2D texSpecMap;
uniform float time;
uniform int numLights;

uniform vec3 cameraPos;

struct Material {
	//TODO: move diffuse texture into struct
	//sampler2D diffuse;
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
	vec3 objectColor = vec3(1,1,1);
	//Ambient
	float ambientStrength = 0.25f;
	//Specular
	float specularStrength = 0.5f;

	//Uncomment one of the next two lines
	vec3 norm = normalize(Normal); //Per vertex normals
	//vec3 norm = normalize(texture(texNormalMap, Normaltexcoord).rgb); //Normal maps

	vec3 ambient = vec3(0);
	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);

	for(int i = 0; i < numLights; i++)
	{
		vec3 lightDir = normalize(lights[i].position - FragPos);

		float diff = max(dot(norm, lightDir), 0.0);

		//Calculate specular lighting
		//I don't really understand that math behind this, should probably learn it
		vec3 viewDir = normalize(cameraPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		ambient  += lights[i].ambient * texture(texDiffuseMap, Texcoord).rgb;
		diffuse  += lights[i].diffuse * (diff * material.diffuse) * texture(texDiffuseMap, Texcoord).rgb;
		specular += lights[i].specular * (spec * material.specular) * texture(texSpecMap, Texcoord).rgb;
	}

	vec3 result = (ambient + diffuse + specular) * objectColor;

	outColor = vec4(result, 1.0f);
}