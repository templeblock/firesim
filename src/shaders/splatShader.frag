#version 330 core

uniform sampler3D inTex;
uniform float slice;
uniform float cellSize;

uniform vec4 center;
uniform float radius;
uniform vec4 force;
uniform float timeStep;

in vec2 Tex;
in vec3 position;

out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, slice * cellSize * .5f);
	vec3 orig = texture(inTex, tex).xyz;
	float exponent = ((position.x - center.x) * (position.x - center.x)) + ((position.y - center.y) * (position.y - center.y)) + ((slice * cellSize - 1.f - center.z) * (slice * cellSize - 1.f - center.z));
	exponent = exponent/radius;
	vec3 splat = force.xyz * timeStep * exp(exponent);
	splat = clamp(splat, -.1f, .1f);
	vec3 res = clamp(orig + splat, -1.f, 1.f);
	FragColor = vec4(res, 1.f);
}