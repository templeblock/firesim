#version 330 core

uniform sampler2D inTex;
uniform vec4 center;
uniform float radius;

in vec2 Tex;
in vec3 position;

out vec4 FragColor;

void main()
{
	float orig = texture(inTex, Tex).x;
	float dist = distance(center.xyz, position);
	float compare = step(dist, radius); //0 if radius < dist, 1 otherwise;
	float res = clamp(orig + compare, 0.f, 1.f);
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}