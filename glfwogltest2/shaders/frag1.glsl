#version 150 core

in vec3 Color;
in vec2 Texcoord;
out vec4 outColor;
uniform sampler2D texKitten;
uniform sampler2D texPuppy;
uniform float time;
void main() {
   outColor = mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), time);
}