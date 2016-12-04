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

uniform Light light;  

void main() {
	outColor = vec4(1.0f);
}