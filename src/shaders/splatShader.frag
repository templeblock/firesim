#version 330 core

uniform sampler2D inTex;
uniform vec4 center;
uniform float radius;
uniform vec4 force;
uniform float timeStep;

in vec2 Tex;
in vec3 position;

out vec4 FragColor;

void main()
{
	vec2 orig = texture(inTex, Tex).xy;
	float exponent = ((position.x - center.x) * (position.x - center.x)) + ((position.y - center.y) * (position.y - center.y));
	exponent = exponent/radius;
	vec2 splat = force.xy * timeStep * exp(exponent);
	splat = clamp(splat, -.1f, .1f);
	vec2 res = clamp(orig + splat, -1.f, 1.f);
	FragColor = vec4(res, 0.f, 1.f);
}