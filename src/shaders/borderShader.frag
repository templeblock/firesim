#version 330 core

uniform sampler3D inTex;
uniform vec4 offset;
uniform float cellSize;
uniform float slice;
uniform float scalar;

in vec2 Tex;

out vec4 FragColor;

void main()
{
	vec3 newTex = vec3(Tex, cellSize * .5f * slice) + offset.xyz;
	vec3 res = texture(inTex, newTex).xyz * scalar;
	FragColor = vec4(res, 1.f);
}