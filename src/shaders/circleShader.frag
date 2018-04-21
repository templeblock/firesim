#version 330 core

uniform sampler2D inTex;
uniform vec4 center;
uniform float radius;

in vec2 Tex;
in vec3 position;

out vec4 FragColor;

void main()
{
	vec2 orig = texture(inTex, Tex).xy;
	float dist = distance(center.xyz, position);
	float compare = step(dist, radius); //0 if radius < dist, 1 otherwise;
	vec2 res = clamp(orig + vec2(compare, compare), 0.f, 1.f);
	FragColor = vec4(res, 0.f, 1.f);
}