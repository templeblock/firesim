#version 330 core

uniform sampler3D inTexture;
uniform float cellSize;

uniform vec4 camPos;
uniform int fireColor;

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
	float red, green, blue;

	switch (fireColor) {
	case 0:
		//REGULAR FLAME	
		red = -119.2 + 31.7 * temp + -.617 * temp * temp + .00282 * temp * temp * temp;
		green = -252 + 32.3 * temp - 0.476 * temp * temp;
		blue = -540.f + 57.9f * temp - 1.27 * temp * temp + .00808 * temp * temp * temp;
		red *= 1.7f;
		green *= 1.5f;
		break;
	case 1:
		//PURPLE FLAME
		red = -85.2 + 26.7 * temp + -.53 * temp * temp + .00253 * temp * temp * temp;
		blue = -42.7 + 8.31 * temp + 0.0711 * temp * temp + -.00253 * temp * temp * temp;
		green = -749.f + 84.7 * temp + -2.388 * temp * temp + .0243 * temp * temp * temp + -.000075 * temp * temp * temp * temp;
		red *= 1.4f;
		blue *= 2.5f;
		break;
	case 2:	
		//TURQUOISE FLAME
		red = -540.f + 57.9f * temp - 1.27 * temp * temp + .00808 * temp * temp * temp;
		green = -252 + 32.3 * temp - 0.476 * temp * temp;
		blue = -119.2 + 31.7 * temp + -.617 * temp * temp + .00282 * temp * temp * temp;
		blue *= 1.7f;
		green *= 1.5f;
		break;
	case 3:
		//NO CHANGE
		red = accumulate.r;
		green = accumulate.g;
		blue = accumulate.b;
	}

	vec3 color = vec3 (red, green, blue);
	color = clamp (color, 0.f, 255.f);
	color = color/255.f;

	FragColor = vec4(color, 1.f);
}