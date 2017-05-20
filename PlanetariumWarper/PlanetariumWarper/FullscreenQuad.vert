#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vUV;
out vec2 UV;
uniform vec2 uvScale;
uniform vec2 uvOffset;

void main()
{
    gl_Position = vPosition;
    UV = vUV * uvScale + uvOffset;
}
