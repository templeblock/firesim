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
  // Note: old pressures are stored in the x component, new ones stored in the y component
  float alpha = -(texture(inPressure, Tex).y - texture(inPressure, Tex).x)
              * (texture(inPressure, Tex).y - texture(inPressure, Tex).x);
  float beta = 1.0f/4.0f;
  float b = texture(divergences, Tex).x;

  vec2 L = clamp(Tex + vec2(-cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 R = clamp(Tex + vec2( cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 T = clamp(Tex + vec2(0.f, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 B = clamp(Tex + vec2(0.f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  float self = texture(inPressure, Tex).y;
  float fL = texture(inPressure, L).y;
  float fR = texture(inPressure, R).y;
  float fT = texture(inPressure, T).y;
  float fB = texture(inPressure, B).y;

	float res = (fL + fR + fB + fT + alpha * b) * beta;
	FragColor = vec4(texture(divergences, Tex).x, res, 1.f, 1.f);
}