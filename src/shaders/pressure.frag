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

  float L = clamp(Tex + vec2(-cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  float R = clamp(Tex + vec2( cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  float T = clamp(Tex + vec2(0.f, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  float B = clamp(Tex + vec2(0.f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  float self = texture(inPressure, Tex).y;
  L = texture(inPressure, L).y;
  R = texture(inPressure, R).y;
  T = texture(inPressure, T).y;
  B = texture(inPressure, B).y;

	float output = (L + R + B + T + alpha * b) * beta;
	FragColor = vec4(vec2(self, output), 1.f, 1.f);
}