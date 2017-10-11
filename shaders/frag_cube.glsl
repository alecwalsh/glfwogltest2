#ifdef GL_ES
precision highp float;
#endif

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
in vec3 FragPos_eye;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texDiffuseMap; //diffuse map
uniform sampler2D texPuppy;
uniform sampler2D texNormalMap;
uniform sampler2D texSpecMap;
uniform float time;
uniform int numPointLights;
uniform int numDirLights;
uniform int numSpotLights;

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
    vec3 direction;
  
    vec3 diffuse;
    vec3 specular;
    float cutoffAngle;
};

uniform Light pointLights[10];
uniform Light dirLights[10];
uniform Light spotLights[10];


void main() {
    outColor = vec4(FragPos_eye.zzz/4, 1.0f);
}
