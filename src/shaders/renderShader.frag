#version 330 core

uniform sampler3D inTexture;
uniform float cellSize;
uniform mat4 object;

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
	vec4 norm = clamp (tangentPoint, vec4(-1.f, -1.f, -1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f));
	norm += vec4(1.f, 1.f, 1.f, 1.f);
	norm /= 2.f;

	vec3 accumulate = vec3(0.f, 0.f, 0.f);

	float upper = max(max(norm.x, norm.y), norm.z);
	float lower = min(min(norm.x, norm.y), norm.z);

	while (upper <= 1.f && lower >= 0.f) {
		accumulate.rgb += texture(inTexture, norm.xyz).rgb * .01;
		tangentPoint += dir * cellSize;
		norm = tangentPoint + vec4(1.f, 1.f, 1.f, 1.f);
		norm /= 2.f;
		upper = max(max(norm.x, norm.y), norm.z);
		lower = min(min(norm.x, norm.y), norm.z);
	}

	//Find Resulting Color
	vec3 color = texture(inTexture, norm.xyz).rgb;

	FragColor = vec4(accumulate * noSect, 0.f);
}