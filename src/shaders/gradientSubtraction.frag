#version 330 core

uniform sampler2D inVelocity;
uniform sampler2D inPressure;
uniform sampler2D divergences;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  float L = clamp(Tex + vec2(-cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  float R = clamp(Tex + vec2( cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  float T = clamp(Tex + vec2(0.f, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  float B = clamp(Tex + vec2(0.f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  L = texture(inPressure, L).y;
  R = texture(inPressure, R).y;
  T = texture(inPressure, T).y;
  B = texture(inPressure, B).y;

  vec2 vel = texture(inVelocity, Tex).xy;

	vec2 output = vel - (vec2((R - L)/2.0f, (T - B)/2.0f) * 0.5f);
	FragColor = vec4(output, 1.f, 1.f);
}