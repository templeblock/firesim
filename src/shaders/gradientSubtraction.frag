#version 330 core

uniform sampler2D inVelocity;
uniform sampler2D inPressure;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  vec2 L = clamp(Tex + vec2(-cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 R = clamp(Tex + vec2( cellSize * .5f, 0.f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 T = clamp(Tex + vec2(0.f, cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 B = clamp(Tex + vec2(0.f, -cellSize * .5f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  float fL = texture(inPressure, L).y;
  float fR = texture(inPressure, R).y;
  float fT = texture(inPressure, T).y;
  float fB = texture(inPressure, B).y;

  vec2 vel = texture(inVelocity, Tex).xy;
  
  vec2 res = vel - (vec2((fR - fL)/2.0f, (fT - fB)/2.0f) * 0.5f);
  FragColor = vec4(res, 0.f, 1.f);
}