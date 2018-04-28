#version 330 core

uniform sampler3D inVelocity;
uniform sampler3D inTemperature;
uniform float cellSize;
uniform float slice;

uniform float speed;
uniform float ambient;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, cellSize * slice);
	float temp = texture(inTemperature, tex).x;
	float buoyancy = clamp(temp - ambient, 0.f, 1.f);
	vec3 res = texture(inVelocity, tex).xyz + vec3(0.f, speed * buoyancy, 0.f);
	res = clamp (res, -1.f, 1.f);
	FragColor = vec4(res, 1.f);
}