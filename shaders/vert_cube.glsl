#ifdef GL_ES
precision highp float;
#endif

in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 texcoord;
in vec3 weight;

out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
out vec2 Texcoord;
out vec3 Weight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    Color = color;

    //Normal matrix
    Normal = mat3(transpose(inverse(model))) * normal; //TODO: This is inefficient, should calculate on CPU

    FragPos = vec3(model * vec4(position, 1.0f)); //Calculate worldspace position
    Texcoord = texcoord;
    Weight = weight;
    vec4 position_tmp = proj * view * model * vec4(position, 1.0);
    gl_Position = vec4(position_tmp.x, position_tmp.y + weight.b*3, position_tmp.zw);
}
