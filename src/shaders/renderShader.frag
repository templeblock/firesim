#version 330 core

uniform sampler3D inTexture;
uniform float cellSize;
uniform mat4 object;
uniform int fireColor;

uniform vec4 camPos;

out vec4 FragColor;
in vec4 position;
in vec2 Tex;

void main()
{
	//Set Up
	mat4 inv = inverse(object);
	vec4 dir = normalize(inv * (position - camPos));
	vec4 o = inv * camPos;

	//Calculate entry time
	float xmin = (-1.f - o.x)/dir.x;
	float xmax = (1.f - o.x)/dir.x;
	float ymin = (-1.f - o.y)/dir.y;
	float ymax = (1.f - o.y)/dir.y;
	float zmin = (-1.f - o.z)/dir.z;
	float zmax = (1.f - o.z)/dir.z;

	float swap = min (xmin, xmax);
	xmax = max(xmin, xmax);
	xmin = swap;

	swap = min (ymin, ymax);
	ymax = max(ymin, ymax);
	ymin = swap;

	swap = min (zmin, zmax);
	zmax = max(zmin, zmax);
	zmin = swap;

	float tmin = max (max (xmin, ymin), zmin);
	float tmax = min (min (xmax, ymax), zmax);
	float noSect = step(tmin, tmax);

	//Tangent Point
	vec4 tangentPoint = o + (dir * tmin);
	vec4 tex = clamp (tangentPoint, vec4(-1.f, -1.f, -1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f));
	tex += vec4(1.f, 1.f, 1.f, 1.f);
	tex *= .5f;

	vec3 accumulate = vec3(0.f, 0.f, 0.f);

	float upper = max(max(tex.x, tex.y), tex.z);
	float lower = min(min(tex.x, tex.y), tex.z);

	int samples;

	while (upper <= 1.f && lower >= 0.f) {
		accumulate.rgb += texture(inTexture, tex.xyz).rgb;
		tangentPoint += dir * cellSize;
		tex = tangentPoint + vec4(1.f, 1.f, 1.f, 1.f);
		tex *= .5f;
		samples++;
		upper = max(max(tex.x, tex.y), tex.z);
		lower = min(min(tex.x, tex.y), tex.z);
	}

	accumulate *= 90.f/samples;

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
		//REGULAR FLAME	
		green = -119.2 + 31.7 * temp + -.617 * temp * temp + .00282 * temp * temp * temp;
		red = -252 + 32.3 * temp - 0.476 * temp * temp;
		blue = -540.f + 57.9f * temp - 1.27 * temp * temp + .00808 * temp * temp * temp;
		green *= 1.7f;
		red *= 1.5f;
		break;
	case 4:
		//NO CHANGE
		red = accumulate.r;
		green = accumulate.g;
		blue = accumulate.b;
	}

	vec3 color = vec3 (red, green, blue);
	color = clamp (color, 0.f, 255.f);
	color = color/255.f;

	FragColor = vec4(color, 0.f);
}