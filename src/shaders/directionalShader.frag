#version 330 core

uniform sampler3D inVelocity;
uniform float cellSize;
uniform float slice;

uniform float time;

out vec4 FragColor;
in vec3 position;
in vec2 Tex;

void main()
{
	vec3 tex = vec3(Tex, cellSize * .5f  * slice);
	vec2 result = texture(inVelocity, tex).xy + vec2(cos(time), sin(time))*.005f;
	result = clamp(result, -1.f, 1.f);
	FragColor = vec4(result, 0.f, 1.f);
}