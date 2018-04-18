#version 330 core

uniform sampler2D input;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	FragColor = texture(inVelocity, Tex);
}