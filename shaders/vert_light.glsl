#ifdef GL_ES
precision highp float;
#endif

//TODO: remove unnecessary attribs after moving lightsObject into separate class
in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texcoord;

out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
out vec2 Texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    Color = color;

    //Normal matrix
    Normal = mat3(transpose(inverse(model))) * normal; //This is inefficient, should calculate on CPU

    FragPos = vec3(1.0);
    Texcoord = texcoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
