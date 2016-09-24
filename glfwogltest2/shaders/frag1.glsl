#version 150 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
//in vec2 Texcoord;

out vec4 outColor;

//uniform sampler2D texKitten;
//uniform sampler2D texPuppy;
uniform float time;
uniform vec3 lightPos;

void main() {
   //outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), time);

   // Ambient
	float ambient = 0.1f;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (ambient + diff) * vec3(1.0, 0.0, 0.0);

	outColor = vec4(diffuse, 1.0f);
}