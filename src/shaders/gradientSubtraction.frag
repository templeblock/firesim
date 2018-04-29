#version 330 core

uniform sampler3D inVelocity;
uniform sampler3D inPressure;
uniform float slice;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3 (Tex, cellSize * .5f  * slice);
	vec3 L = clamp(tex + vec3(-cellSize * .5f, 0.f, 0.f), 0.f, 1.f);
	vec3 R = clamp(tex + vec3( cellSize * .5f, 0.f, 0.f), 0.f, 1.f);
	vec3 T = clamp(tex + vec3(0.f,  cellSize * .5f, 0.f), 0.f, 1.f);
	vec3 B = clamp(tex + vec3(0.f, -cellSize * .5f, 0.f), 0.f, 1.f);
	vec3 U = clamp(tex + vec3(0.f, 0.f,  cellSize * .5f), 0.f, 1.f);
	vec3 D = clamp(tex + vec3(0.f, 0.f, -cellSize * .5f), 0.f, 1.f);

	float fL = texture(inPressure, L).x;
	float fR = texture(inPressure, R).x;
	float fT = texture(inPressure, T).x;
	float fB = texture(inPressure, B).x;
	float fU = texture(inPressure, U).x;
	float fD = texture(inPressure, D).x;

	vec3 vel = texture(inVelocity, tex).xyz;
  
	vec3 res = vel - (vec3((fR - fL)/2.0f, (fT - fB)/2.0f, (fU - fD)/2.0f) * 0.5f);
	res = clamp (res, -1.f, 1.f);
	FragColor = vec4(res, 1.f);
}