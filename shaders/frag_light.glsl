#ifdef GL_ES
precision highp float;
#endif

in vec3 Color;
in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0f);
}
