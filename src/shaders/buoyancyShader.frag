#version 330 core

uniform sampler2D inVelocity;
uniform sampler2D inTemperature;

uniform float speed;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 res = texture(inVelocity, Tex).xyz + vec3(0.f, speed * 0.05f, 0.f);
	FragColor = vec4(res, 1.f);
}