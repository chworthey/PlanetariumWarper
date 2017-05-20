#version 330 core

in vec2 UV;

void main()
{
    gl_FragColor = vec4(UV.r, UV.g, 0.0, 1.0);
}