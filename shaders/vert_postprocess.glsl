#ifdef GL_ES
precision highp float;
#endif

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texcoord;

out vec2 Texcoord;

void main() {
    Texcoord = texcoord;
    gl_Position = vec4(position, 1.0);
}
