#version 330 core

uniform sampler3D inTex;
uniform sampler3D inRand;
uniform float cellSize;
uniform float slice;
uniform float texRow;

uniform vec4 center;
uniform float radius;

in vec2 Tex;
in vec3 position;

out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, slice * cellSize * .5f);
	float orig = texture(inTex, tex).x;
	vec3 pos = vec3(position.xy, (slice * cellSize) - 1.f);
	float dist = distance(center.xyz, pos);
	float compare = step(dist, radius); //0 if radius < dist, 1 otherwise;
	if (Tex.y < .08f && Tex.y > 0.f) {
		compare += texture(inRand, tex).x * Tex.y * 5.f;
	}
	float res = min(orig + compare, 1.0f);
	FragColor = vec4(res, 0.f, 0.f, 1.f);
}