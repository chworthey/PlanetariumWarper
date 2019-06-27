#version 330 core

in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;

out vec4 col;

void main(void) {
  col = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;
}