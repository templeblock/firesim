#version 330 core

uniform sampler2D inVelocity;
uniform float time;

out vec4 FragColor;
in vec3 position;
in vec2 Tex;

void main()
{
	vec2 result = texture(inVelocity, Tex).xy + vec2(cos(time), sin(time))*.005f;
	FragColor = vec4(result, 0.f, 1.f);
}