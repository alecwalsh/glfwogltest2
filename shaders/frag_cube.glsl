#ifdef GL_ES
precision highp float;
#endif

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texDiffuseMap; //diffuse map
uniform sampler2D texSpecMap;
uniform sampler2D texNormalMap;
uniform sampler2D texFramebuffer;
uniform float time;

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

const int POINTLIGHT = 0;
const int DIRECTIONALLIGHT = 1;
const int SPOTLIGHT = 2;

struct Light {
    int type; // 0 for point light, 1 for directional light, 2 for spotlight
    vec3 position;
    vec3 direction;
  
    vec3 diffuse;
    vec3 specular;
    float cutoffAngle;
};

uniform int numLights;
uniform Light lights[30];


void calculateLighting(Light light, vec3 norm, out vec3 diffuse, out vec3 specular) {
    vec3 lightDir = normalize(light.position - FragPos);
    
    float dist = distance(light.position, FragPos);
    //TODO: Implement attenuation  properly
    float attenuation = 15.0 / pow(dist, 2.0);
    
    // Do calculations that are different for different types of lights here
    switch(light.type) {
        case POINTLIGHT:
            break;
        case DIRECTIONALLIGHT:
            {
                lightDir = -normalize(light.direction);
                attenuation = 1.0;
            }
            break;
        case SPOTLIGHT:
            {
                float theta = dot(lightDir, normalize(-light.direction));
                if(theta <= light.cutoffAngle) {
                    // If theta > light.cutoffAngle, then the spotlight affects this fragment
                    // This sets diffuse and specular to 0 if the fragment is not affected by the spotlight
                    // diffuse and specular are both multiplied by attenuation later, so setting attenuation to 0 ensures they are set to 0 too
                    attenuation = 0.0;
                }
            }
            break;
    }


    float diff = max(dot(norm, lightDir), 0.0);

    //Calculate specular lighting
    //I don't really understand that math behind this, should probably learn it
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    diffuse += light.diffuse * (diff * material.diffuse) * texture2D(texDiffuseMap, Texcoord).rgb * attenuation;
    specular += light.specular * (spec * material.specular) * texture2D(texSpecMap, Texcoord).rgb * attenuation;
}


void main() {
    vec3 objectColor = vec3(1,1,1);

    //Uncomment one of the next two lines
    vec3 norm = normalize(Normal); //Per vertex normals
    //vec3 norm = normalize(texture(texNormalMap, Normaltexcoord).rgb); //Normal maps

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    
    for(int i = 0; i < numLights; i++) {
        calculateLighting(lights[i], norm, diffuse, specular);
    }

    vec3 ambient = uniAmbient * texture2D(texDiffuseMap, Texcoord).rgb;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    outColor = vec4(result, 1.0f);
}
