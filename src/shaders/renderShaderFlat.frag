#version 330 core

uniform sampler3D inTexture;
uniform float cellSize;

uniform vec4 camPos;

out vec4 FragColor;
in vec4 position;
in vec2 Tex;

void main()
{
	vec3 tex = vec3(Tex, 0.f);
	vec3 accumulate = vec3(0.f, 0.f, 0.f);

	while (tex.z < 1.f) {
		accumulate += texture(inTexture, tex).rgb;
		tex.z += cellSize * .5f;
	}

	accumulate *= cellSize * 1.5f;

	FragColor = vec4(accumulate, 1.f);
}