#version 330 core

uniform sampler3D inPressure;
uniform sampler3D divergences;
uniform float slice;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, cellSize * slice);
	float alpha = cellSize * cellSize;
	float beta = .25f;
	float b = texture(divergences, tex).x;

	vec3 L = clamp(tex + vec3(-cellSize * .5f, 0.f, 0.f), 0.f, 1.f);
	vec3 R = clamp(tex + vec3( cellSize * .5f, 0.f, 0.f), 0.f, 1.f);
	vec3 T = clamp(tex + vec3(0.f,  cellSize * .5f, 0.f), 0.f, 1.f);
	vec3 B = clamp(tex + vec3(0.f, -cellSize * .5f, 0.f), 0.f, 1.f);
	vec3 U = clamp(tex + vec3(0.f, 0.f,  cellSize * .5f), 0.f, 1.f);
	vec3 D = clamp(tex + vec3(0.f, 0.f, -cellSize * .5f), 0.f, 1.f);

	float self = texture(inPressure, tex).x;
	float fL = texture(inPressure, L).x;
	float fR = texture(inPressure, R).x;
	float fT = texture(inPressure, T).x;
	float fB = texture(inPressure, B).x;
	float fU = texture(inPressure, U).x;
	float fD = texture(inPressure, D).x;

	float res = (fL + fR + fB + fT + fU + fD - alpha * b) * beta;
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}