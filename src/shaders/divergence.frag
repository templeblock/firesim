#version 330 core

uniform sampler2D inVelocity;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  vec2 L = clamp(Tex + vec2(-cellSize * .5f, 0), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 R = clamp(Tex + vec2( cellSize * .5f, 0), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 T = clamp(Tex + vec2(0, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 B = clamp(Tex + vec2(0, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  vec2 self = texture(inVelocity, Tex).xy;
  vec2 L_vel = texture(inVelocity, L).xy;
  vec2 R_vel = texture(inVelocity, R).xy;
  vec2 T_vel = texture(inVelocity, T).xy;
  vec2 B_vel = texture(inVelocity, B).xy;

	float res = ((R_vel.x - L_vel.x) + (T_vel.y - B_vel.y))/2.0f;
	FragColor = vec4(res, 1.f, 1.f, 1.f);
}