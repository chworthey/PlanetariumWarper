#version 330 core

uniform sampler2D texSampler;
in vec2 UV;
out vec3 color;

void main()
{
    color = texture(texSampler, UV).rgb;
}