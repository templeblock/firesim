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
  vec2 alpha = ((texture(inVelocity, Tex) - texture(inVelocity, pastPos))
              * (texture(inVelocity, Tex) - texture(inVelocity, pastPos)))/(timeStep/viscosity);
  vec2 beta = vec2(1.0f/(4.0f + alpha.x), 1.0f/(4.0f + alpha.y));

	vec2 L = clamp(Tex + vec2(-cellSize * .5f, 0), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R = clamp(Tex + vec2( cellSize * .5f, 0), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 T = clamp(Tex + vec2(0, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 B = clamp(Tex + vec2(0, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  vec2 self = texture(inVelocity, Tex).xy;
  vec2 L_vel = texture(inVelocity, L).xy;
  vec2 R_vel = texture(inVelocity, R).xy;
  vec2 T_vel = texture(inVelocity, T).xy;
  vec2 B_vel = texture(inVelocity, B).xy;

  output = (L_vel + R_vel + T_vel + B_vel + alpha * self) * beta;

	FragColor = vec4(output, 1.f);
}