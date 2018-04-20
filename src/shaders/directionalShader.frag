#version 330 core

uniform vec4 source;

out vec4 FragColor;
in vec3 position;

void main()
{
	vec3 direction = position - source.xyz;
	direction = direction*5;
	FragColor = vec4(direction, 1.f);
}