#version 330 core

uniform sampler2D inVelocity;
uniform sampler2D inTemperature;

uniform float speed;
uniform float ambient;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	float temp = texture(inTemperature, Tex).x;
	float buoyancy = clamp(temp - ambient, 0.f, 1.f);
	vec3 res = texture(inVelocity, Tex).xyz + vec3(0.f, speed * buoyancy, 0.f);
	res = clamp (res, -1.f, 1.f);
	FragColor = vec4(res, 1.f);
}