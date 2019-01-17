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
uniform mat4 bone_transforms[MAX_BONES];


mat4 rotationMatrix(vec3 axis, float theta) {
    vec3 a = normalize(axis);

    float x = a.x;
    float y = a.y;
    float z = a.z;

    float s = sin(theta);
    float c = cos(theta);

    return mat4(
                x * x * (1-c) + c,        x * y * (1-c) - z * s,   x * z * (1-c) + y * s, 0,
                x * y * (1-c) + z * s,    y * y * (1-c) + c,       y * z * (1-c) - x * s, 0,
                x * z * (1-c) - y * s,    y * z * (1-c) + x * s,   z * z * (1-c) + c,     0,
                0,                        0,                       0,                     1
    );

}

void main() {
    Color = color;

    //Normal matrix
    Normal = mat3(transpose(inverse(model))) * normal; //TODO: This is inefficient, should calculate on CPU
    
    Texcoord = texcoord;
    Weight = weight;
    
    //Set the rotation amount using  the time and weight for bone 0
    mat4 rotm = mat4(1);//rotationMatrix(vec3(0.0,0.0,1.0), weight.x);
    
    vec4 transformed_position = vec4(position, 1.0);
    
    //Bone 0 is more significant
    if(weight.x > weight.y && weight.x > weight.z) {
        transformed_position = bone_transforms[0] * vec4(position, 1.0);
//         transformed_position = bone_transforms[1] * transformed_position;
//         transformed_position = bone_transforms[2] * transformed_position;
    }
    
    //Bone 1 is more significant
    if(weight.y > weight.x && weight.y > weight.z) {
        transformed_position = bone_transforms[0] * vec4(position, 1.0);
        transformed_position = bone_transforms[1] * transformed_position;
//         transformed_position = bone_transforms[2] * transformed_position;
    }
    
    if(weight.z > weight.x && weight.z > weight.y) {
        transformed_position = bone_transforms[0] * vec4(position, 1.0);
        transformed_position = bone_transforms[1] * transformed_position;
        transformed_position = bone_transforms[2] * transformed_position;
    }
    
    //Apply the rotation
    transformed_position = rotm * vec4(transformed_position.xyz, 1.0);
    
    FragPos = vec3(model * transformed_position); //Calculate worldspace position
    
    gl_Position = proj * view * model * vec4(transformed_position.xyz, 1.0);
}
