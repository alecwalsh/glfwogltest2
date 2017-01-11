#version 150 core


//TODO: remove unnecessary attribs after moving lightsObject into separate class
in vec3 position;
in vec3 color;
in vec3 normal;
in vec2 normaltexcoord;

out vec3 Color;
out vec3 Normal;
out vec3 FragPos;
out vec2 Normaltexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	Color = color;

	//Normal matrix
	Normal = mat3(transpose(inverse(model))) * normal; //This is inefficient, should calculate on CPU

	FragPos = vec3(1.0);
	Normaltexcoord = normaltexcoord;
	gl_Position = proj * view * model * vec4(position, 1.0);
}