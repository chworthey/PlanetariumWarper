#version 330 core

uniform sampler2D texSampler;
uniform sampler2D warpSampler;
in vec2 UV;
out vec3 color;

void main()
{
    vec3 warp = texture(warpSampler, UV).rgb;
    color = texture(texSampler, warp.rg).rgb * warp.b;
}