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
	vec3 tex = vec3(Tex, slice * cellSize);
	vec2 orig = texture(inTex, tex).xy;
	float exponent = ((position.x - center.x) * (position.x - center.x)) + ((position.y - center.y) * (position.y - center.y));
	exponent = exponent/radius;
	vec2 splat = force.xy * timeStep * exp(exponent);
	splat = clamp(splat, -.1f, .1f);
	vec2 res = clamp(orig + splat, -1.f, 1.f);
	FragColor = vec4(res, 0.f, 1.f);
}