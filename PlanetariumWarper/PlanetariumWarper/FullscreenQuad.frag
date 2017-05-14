#version 130

precision highp float;

in vec2 TexCoord;

out vec4 FragColor;

void main()
{
	// In this demo we just output a gray colored full screen quad.
	// A real application will most likely require 1 or more input textures
	// for performing tasks such as post processing and compositing.
    
	FragColor = vec4(1.0, 0.5, 0.5, 1.0);
}