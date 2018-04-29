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
	vec3 tex = vec3(Tex, slice * cellSize); //get slice that you are sampling from
	vec3 pastPos = tex - (texture(inVelocity, tex).xyz * timeStep);
	pastPos = clamp(pastPos, 0.f, 1.f);

	vec3 LBU = clamp(pastPos + vec3(-cellSize * .25f, -cellSize * .25f,  cellSize * .25f), 0.f, 1.f);
	vec3 RBU = clamp(pastPos + vec3( cellSize * .25f, -cellSize * .25f,  cellSize * .25f), 0.f, 1.f);
	vec3 LTU = clamp(pastPos + vec3(-cellSize * .25f,  cellSize * .25f,  cellSize * .25f), 0.f, 1.f);
	vec3 RTU = clamp(pastPos + vec3( cellSize * .25f,  cellSize * .25f,  cellSize * .25f), 0.f, 1.f);

	vec3 LBD = clamp(pastPos + vec3(-cellSize * .25f, -cellSize * .25f, -cellSize * .25f), 0.f, 1.f);
	vec3 RBD = clamp(pastPos + vec3( cellSize * .25f, -cellSize * .25f, -cellSize * .25f), 0.f, 1.f);
	vec3 LTD = clamp(pastPos + vec3(-cellSize * .25f,  cellSize * .25f, -cellSize * .25f), 0.f, 1.f);
	vec3 RTD = clamp(pastPos + vec3( cellSize * .25f,  cellSize * .25f, -cellSize * .25f), 0.f, 1.f);

	vec3 LB = mix(texture(inQuantity, LBU).rgb, texture(inQuantity, LBD).rgb, .5f);
	vec3 RB = mix(texture(inQuantity, RBU).rgb, texture(inQuantity, RBD).rgb, .5f);
	vec3 LT = mix(texture(inQuantity, LTU).rgb, texture(inQuantity, LTD).rgb, .5f);
	vec3 RT = mix(texture(inQuantity, RTU).rgb, texture(inQuantity, RTD).rgb, .5f);
	
	vec3 L = mix(LB, LT, .5f);
	vec3 R = mix(RB, RT, .5f);

	vec3 res = mix(L, R, 0.5f);

	res = clamp (res, -1.f, 1.f);
	FragColor = vec4(res, 1.f);
}