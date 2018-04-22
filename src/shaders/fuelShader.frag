#version 330 core

uniform sampler2D inQuantity;

uniform float rate;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	float res = texture(inQuantity, Tex).x;
	res = res - rate * timeStep;
	res = clamp (res, -1.f, 1.f);
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}