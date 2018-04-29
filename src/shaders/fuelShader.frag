#version 330 core

uniform sampler3D inQuantity;
uniform float cellSize;
uniform float slice;

uniform float rate;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, cellSize * slice * .5f);
	float res = texture(inQuantity, tex).x;
	res = res - (rate * timeStep);
	res = clamp (res, 0.f, 1.0f);
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}