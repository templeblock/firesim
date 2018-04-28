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
	vec2 newTex = Tex + offset.xy;
	vec3 tex = vec3(newTex, cellSize * slice);
	vec3 res = texture(inTex, tex).xyz * scalar;
	FragColor = vec4(res, 1.f);
}