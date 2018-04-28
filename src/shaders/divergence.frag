#version 330 core

uniform sampler3D inVelocity;
uniform float slice;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, cellSize * slice);
	vec3 L = tex + vec3(-cellSize * .5f, 0.f, 0.f);
	vec3 R = tex + vec3( cellSize * .5f, 0.f, 0f);
	vec3 T = tex + vec3(0.f,  cellSize * .5f, 0.f);
	vec3 B = tex + vec3(0.f, -cellSize * .5f, 0.f);

	vec2 self =  texture(inVelocity, tex).xy;
	vec2 L_vel = texture(inVelocity, L).xy;
	vec2 R_vel = texture(inVelocity, R).xy;
	vec2 T_vel = texture(inVelocity, T).xy;
	vec2 B_vel = texture(inVelocity, B).xy;

	float res = (R_vel.x - L_vel.x) + (T_vel.y - B_vel.y);
	res = res * 0.5f;
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}