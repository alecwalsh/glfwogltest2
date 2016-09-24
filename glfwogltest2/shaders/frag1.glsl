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

void main() {
   //outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), time);

	//Ambient
	float ambient = 0.1f;
	
	//Uncomment one of the next two lines
	vec3 norm = normalize(Normal); //Per vertex normals
	//vec3 norm = normalize(texture(texNormalMap, Normaltexcoord).rgb); //Normal maps
	
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (ambient + diff) * vec3(1.0, 0.0, 1.0);

	outColor = vec4(diffuse, 1.0f);
}