#version 330 core

uniform sampler2D inVelocity;

uniform float viscosity;
uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	float alpha = (cellSize * cellSize)/(viscosity * timeStep);
	float beta = 1.f/(4.f+ alpha);
	vec2 L = clamp(Tex + vec2(-cellSize * .5, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R = clamp(Tex + vec2( cellSize * .5, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 T = clamp(Tex + vec2(0.f, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 B = clamp(Tex + vec2(0.f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

	vec3 self = texture(inVelocity, Tex).xyz;
	vec3 L_vel = texture(inVelocity, L).xyz;
	vec3 R_vel = texture(inVelocity, R).xyz;
	vec3 T_vel = texture(inVelocity, T).xyz;
	vec3 B_vel = texture(inVelocity, B).xyz;

	vec3 res = (L_vel + R_vel + T_vel + B_vel + alpha * self).xyz * beta;

	FragColor = vec4(res, 1.f);
}