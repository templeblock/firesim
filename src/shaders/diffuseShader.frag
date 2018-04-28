#version 330 core

uniform sampler3D inVelocity;
uniform float slice;

uniform float viscosity;
uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, slice * cellSize);
	float alpha = (cellSize * cellSize)/(viscosity * timeStep);
	float beta = 1.f/(4.f+ alpha);
	vec3 L = clamp(tex + vec3(-cellSize * .5, 0.f, 0.f), 0.f, 1.f);
	vec3 R = clamp(tex + vec3( cellSize * .5, 0.f, 0.f), 0.f, 1.f);
	vec3 T = clamp(tex + vec3(0.f, cellSize * .5f, 0.f), 0.f, 1.f);
	vec3 B = clamp(tex + vec3(0.f, -cellSize * .5f, 0.f), 0.f, 1.f);

	vec3 U = clamp(tex + vec3(0.f, 0.f,  cellSize * .5f), 0.f, 1.f); //front
	vec3 D = clamp(tex + vec3(0.f, 0.f, -cellSize * .5f), 0.f, 1.f); //back

	vec3 self = texture(inVelocity, tex).xyz;
	vec3 L_vel = texture(inVelocity, L).xyz;
	vec3 R_vel = texture(inVelocity, R).xyz;
	vec3 T_vel = texture(inVelocity, T).xyz;
	vec3 B_vel = texture(inVelocity, B).xyz;

	vec3 U_vel = texture(inVelocity, U).xyz;
	vec3 D_vel = texture(inVelocity, D).xyz;



	vec3 res = (L_vel + R_vel + T_vel + B_vel + U_vel + D_vel + alpha * self).xyz * beta;

	FragColor = vec4(res, 1.f);
}