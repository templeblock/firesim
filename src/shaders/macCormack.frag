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
	//Backward
	vec3 tex = vec3(Tex, slice * cellSize);
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

	float3 phiMin = min(min(min(min(min(min(min(LBU,  RBU), LTU), RTU), LBD), RBD), LTD), RTD);
	float3 phiMax = max(max(max(max(max(max(max(LBU,  RBU), LTU), RTU), LBD), RBD), LTD), RTD);

	vec3 LB = mix(texture(inQuantity, LBU).rgb, texture(inQuantity, LBD).rgb, .5f);
	vec3 RB = mix(texture(inQuantity, RBU).rgb, texture(inQuantity, RBD).rgb, .5f);
	vec3 LT = mix(texture(inQuantity, LTU).rgb, texture(inQuantity, LTD).rgb, .5f);
	vec3 RT = mix(texture(inQuantity, RTU).rgb, texture(inQuantity, RTD).rgb, .5f);
	
	vec3 L = mix(LB, LT, .5f);
	vec3 R = mix(RB, RT, .5f);

	vec3 res = mix(L, R, 0.5f);

	//Forward
	vec2 nextPos = pastPos + (texture(inVelocity, pastPos).xy * timeStep);
	nextPos = clamp(nextPos, vec2(0.f, 0.f), vec2(1.f, 1.f));

	vec3 LBU2 = clamp(nextPos + vec3(-cellSize * .25f, -cellSize * .25f,  cellSize * .25f), 0.f, 1.f);
	vec3 RBU2 = clamp(nextPos + vec3( cellSize * .25f, -cellSize * .25f,  cellSize * .25f), 0.f, 1.f);
	vec3 LTU2 = clamp(nextPos + vec3(-cellSize * .25f,  cellSize * .25f,  cellSize * .25f), 0.f, 1.f);
	vec3 RTU2 = clamp(nextPos + vec3( cellSize * .25f,  cellSize * .25f,  cellSize * .25f), 0.f, 1.f);

	vec3 LBD2 = clamp(nextPos + vec3(-cellSize * .25f, -cellSize * .25f, -cellSize * .25f), 0.f, 1.f);
	vec3 RBD2 = clamp(nextPos + vec3( cellSize * .25f, -cellSize * .25f, -cellSize * .25f), 0.f, 1.f);
	vec3 LTD2 = clamp(nextPos + vec3(-cellSize * .25f,  cellSize * .25f, -cellSize * .25f), 0.f, 1.f);
	vec3 RTD2 = clamp(nextPos + vec3( cellSize * .25f,  cellSize * .25f, -cellSize * .25f), 0.f, 1.f);

	vec3 LB2 = mix(texture(inQuantity, LBU2).rgb, texture(inQuantity, LBD2).rgb, .5f);
	vec3 RB2 = mix(texture(inQuantity, RBU2).rgb, texture(inQuantity, RBD2).rgb, .5f);
	vec3 LT2 = mix(texture(inQuantity, LTU2).rgb, texture(inQuantity, LTD2).rgb, .5f);
	vec3 RT2 = mix(texture(inQuantity, RTU2).rgb, texture(inQuantity, RTD2).rgb, .5f);
	
	vec3 L2 = mix(LB2, LT2, .5f);
	vec3 R2 = mix(RB2, RT2, .5f);

	vec3 res2 = mix(L2, R2, 0.5f);

	res = res + 0.5f * (texture(inVelocity, tex).xyz - res2);
	vec3 ret = clamp(res.xy, phiMin, phiMax);
	FragColor = vec4(ret, 1.f);
}