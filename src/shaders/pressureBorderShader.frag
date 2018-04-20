#version 330 core

uniform sampler2D inPressure;
uniform float cellSize;

in vec3 position;

out vec4 FragColor;

void main()
{
	vec2 newTex = position.xy * (1.f - cellSize);
	newTex = newTex + vec2(1.f, 1.f);
	newTex = newTex/2.f;
	FragColor = texture(inPressure, newTex);
}