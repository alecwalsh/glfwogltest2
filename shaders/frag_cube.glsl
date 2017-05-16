#version 330

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
uniform int numPointLights;
uniform int numDirLights;

uniform vec3 cameraPos;

uniform vec3 uniAmbient;

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
  
    vec3 diffuse;
    vec3 specular;
};

uniform Light pointLights[10];
uniform Light dirLights[10];


void main() {
	Light light = pointLights[0];
   //outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), time);
	vec3 objectColor = vec3(1,1,1);
	//Ambient
	float ambientStrength = 0.25f;
	//Specular
	float specularStrength = 0.5f;

	//Uncomment one of the next two lines
	vec3 norm = normalize(Normal); //Per vertex normals
	//vec3 norm = normalize(texture(texNormalMap, Normaltexcoord).rgb); //Normal maps

	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);

	//TODO: lots of repetition, should move lighting into separate function
	//Calculate point lights
	for(int i = 0; i < numPointLights; i++)
	{
		vec3 lightDir = normalize(pointLights[i].position - FragPos);

		float diff = max(dot(norm, lightDir), 0.0);

		//Calculate specular lighting
		//I don't really understand that math behind this, should probably learn it
		vec3 viewDir = normalize(cameraPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		diffuse  += pointLights[i].diffuse * (diff * material.diffuse) * texture(texDiffuseMap, Texcoord).rgb;
		specular += pointLights[i].specular * (spec * material.specular) * texture(texSpecMap, Texcoord).rgb;
	}
	
	//Now calculate directional lights
    for(int i = 0; i < numDirLights; i++)
	{
		vec3 lightDir = -normalize(dirLights[i].position); //Using position as direction here

		float diff = max(dot(norm, lightDir), 0.0);

		//Calculate specular lighting
		//I don't really understand that math behind this, should probably learn it
		vec3 viewDir = normalize(cameraPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		diffuse  += dirLights[i].diffuse * (diff * material.diffuse) * texture(texDiffuseMap, Texcoord).rgb;
		specular += dirLights[i].specular * (spec * material.specular) * texture(texSpecMap, Texcoord).rgb;
	}

	vec3 ambient = uniAmbient * texture(texDiffuseMap, Texcoord).rgb;
	
	vec3 result = (ambient + diffuse + specular) * objectColor;

	outColor = vec4(result, 1.0f);
}
