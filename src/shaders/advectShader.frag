#version 330 core

uniform sampler3D inVelocity;
uniform sampler3D inQuantity;
uniform float slice;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 tex = vec3(Tex, slice * cellSize);
	vec3 pastPos = tex - (texture(inVelocity, tex).xyz * timeStep);
	pastPos = clamp(pastPos, 0.f, 1.f);

	vec3 L_B = clamp(pastPos + vec3(-cellSize * .25f, -cellSize * .25f, 0.f), 0.f, 1.f);
	vec3 R_B = clamp(pastPos + vec3( cellSize * .25f, -cellSize * .25f, 0.f), 0.f, 1.f);
	vec3 L_T = clamp(pastPos + vec3(-cellSize * .25f,  cellSize * .25f, 0.f), 0.f, 1.f);
	vec3 R_T = clamp(pastPos + vec3( cellSize * .25f,  cellSize * .25f, 0.f), 0.f, 1.f);

	//TODO: implement centroid calculations for t
	vec3 colorB = mix(texture(inQuantity, L_B).rgb, texture(inQuantity, R_B).rgb, .5f);
	vec3 colorT = mix(texture(inQuantity, L_T).rgb, texture(inQuantity, R_T).rgb, .5f);

	vec3 res = mix(colorB, colorT, 0.5f);
	res = clamp (res, -1.f, 1.f);
	FragColor = vec4(res, 1.f);
}