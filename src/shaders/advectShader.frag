#version 330 core

uniform sampler2D inVelocity;

uniform float cellSize;
uniform float timeStep;
uniform float dye;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec2 pastPos = Tex - texture(inVelocity, Tex).xy * timeStep;
	pastPos = clamp(pastPos, vec2(0.f, 0.f), vec2(1.f, 1.f));

	vec2 L_B = clamp(pastPos + vec2(-cellSize * .25f, -cellSize * .25f), vec2(-1.f, -1.f), vec2(1.f, 1.f));
	vec2 R_B = clamp(pastPos + vec2( cellSize * .25f, -cellSize * .25f), vec2(-1.f, -1.f), vec2(1.f, 1.f));
	vec2 L_T = clamp(pastPos + vec2(-cellSize * .25f,  cellSize * .25f), vec2(-1.f, -1.f), vec2(1.f, 1.f));
	vec2 R_T = clamp(pastPos + vec2( cellSize * .25f,  cellSize * .25f), vec2(-1.f, -1.f), vec2(1.f, 1.f));

	//TODO: implement centroid calculations for t
	vec3 colorB = mix(texture(inVelocity, L_B).rgb, texture(inVelocity, R_B).rgb, .5f);
	vec3 colorT = mix(texture(inVelocity, L_T).rgb, texture(inVelocity, R_T).rgb, .5f);

	vec3 output = mix(colorB, colorT, 0.5f) * dye;
	vec3 normalized = normalize(output);
	FragColor = vec4(output, 1.f);
}