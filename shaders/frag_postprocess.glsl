#ifdef GL_ES
precision highp float;
#endif

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texFramebuffer;

void main() {
    outColor = texture(texFramebuffer, Texcoord);
}
