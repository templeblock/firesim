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

	accumulate *= cellSize * 75.f;

	float temp = accumulate.r;
	temp = temp * 1.2f;
	float red = -85.2 + 26.7 * temp + -.53 * temp * temp + .00253 * temp * temp * temp;
	float green = -42.7 + 8.31 * temp + 0.0711 * temp * temp + -.00253 * temp * temp * temp;
	float blue = -749.f + 84.7 * temp + -2.388 * temp * temp + .0243 * temp * temp * temp + -.000075 * temp * temp * temp * temp;
	vec3 color = vec3 (red, green, blue);
	color = clamp (color, 0.f, 255.f);
	color = color/255.f;

	FragColor = vec4(color, 1.f);
}