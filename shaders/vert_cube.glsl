#ifdef GL_ES
precision highp float;
#endif

in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texcoord;
in vec4 weight;

out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
out vec2 Texcoord;
out vec4 Weight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float time;

#define MAX_BONES 5
uniform mat4 bone_matrices[MAX_BONES];


mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {
    Color = color;

    //Normal matrix
    Normal = mat3(transpose(inverse(model))) * normal; //TODO: This is inefficient, should calculate on CPU
    
    Texcoord = texcoord;
    Weight = weight;
    
    //Set the rotation amount using  the time and weight for bone 0
    mat4 rotm = rotationMatrix(vec3(0.0,0.0,1.0), time*weight.x);
    
    //Apply the rotation
    vec4 transformed_position = inverse(bone_matrices[0]) * rotm * bone_matrices[0] * vec4(position, 1.0);
    
    FragPos = vec3(model * transformed_position); //Calculate worldspace position
    
    gl_Position = proj * view * model * transformed_position;
}
