#version 330 core

uniform sampler2D input;
uniform sampler2D dXdT;

uniform float cellSize;
uniform float viscosity;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  vec2 pastPos = Tex - texture(input, Tex).xy * timeStep;
  pastPos = clamp(pastPos, vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 alpha = pow(texture(input, Tex) - texture(input, pastPos), 2.0f)/(timeStep/viscosity);
  vec2 beta = vec2(1.0f/(4.0f + alpha.x), 1.0f/(4.0f + alpha.y));

	vec2 L_B = clamp(Tex + vec2(-cellSize * .25f, -cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R_B = clamp(Tex + vec2( cellSize * .25f, -cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 L_T = clamp(Tex + vec2(-cellSize * .25f,  cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R_T = clamp(Tex + vec2( cellSize * .25f,  cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  vec2 self = texture(input, Tex).xy;
  vec2 L_B_vel = texture(input, L_B).xy;
  vec2 R_B_vel = texture(input, R_B).xy;
  vec2 L_T_vel = texture(input, L_T).xy;
  vec2 R_T_vel = texture(input, R_T).xy;

  output = (L_B_vel + R_B_vel + L_T_vel + R_T_vel + alpha * self) * beta;

	FragColor = vec4(output, 1.f);
}