#version 330 core

uniform sampler3D inTex;
uniform float cellSize;
uniform float slice;

uniform vec4 center;
uniform float radius;

in vec2 Tex;
in vec3 position;

out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, slice * cellSize);
	float orig = texture(inTex, tex).x;
	float dist = distance(center.xyz, position);
	float compare = step(dist, radius); //0 if radius < dist, 1 otherwise;
	float res = min(orig + compare, 1.f);
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}