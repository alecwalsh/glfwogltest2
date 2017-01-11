#version 150 core

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;
in vec2 Normaltexcoord;

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
	outColor = vec4(1.0f);
}