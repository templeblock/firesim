#version 330 core

uniform sampler2D inVelocity;
//uniform sampler2D centroids;

uniform float cellSize;
uniform float viscosity;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  vec2 pastPos = Tex - texture(inVelocity, Tex).xy * timeStep;
  pastPos = clamp(pastPos, vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 alpha = ((texture(inVelocity, Tex) - texture(inVelocity, pastPos))
              * (texture(inVelocity, Tex) - texture(inVelocity, pastPos)))/(timeStep/viscosity);
  vec2 beta = vec2(1.0f/(4.0f + alpha.x), 1.0f/(4.0f + alpha.y));

	vec2 L_B = clamp(Tex + vec2(-cellSize * .5f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R_B = clamp(Tex + vec2( cellSize * .5f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 L_T = clamp(Tex + vec2(-cellSize * .5f,  cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R_T = clamp(Tex + vec2( cellSize * .5f,  cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  vec2 self = texture(inVelocity, Tex).xy;
  vec2 L_B_vel = texture(inVelocity, L_B).xy;
  vec2 R_B_vel = texture(inVelocity, R_B).xy;
  vec2 L_T_vel = texture(inVelocity, L_T).xy;
  vec2 R_T_vel = texture(inVelocity, R_T).xy;

	//TODO: implement centroid calculations for t

  output = (L_B_vel + R_B_vel + L_T_vel + R_T_vel + alpha * self) * beta;

	FragColor = vec4(output, 1.f);
}