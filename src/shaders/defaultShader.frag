#version 330 core

uniform sampler2D input;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec2 pastPos = Tex - .xy;
	FragColor = texture(inVelocity, Tex);
}