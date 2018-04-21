#version 330 core

uniform sampler2D inVelocity;
uniform sampler2D inQuantity;

uniform float cellSize;
uniform float timeStep;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  // normal advection step
	vec2 pastPos = Tex - (texture(inVelocity, Tex).xy * timeStep);
	pastPos = clamp(pastPos, vec2(0.f, 0.f), vec2(1.f, 1.f));

	vec2 L_B = clamp(pastPos + vec2(-cellSize * .25f, -cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R_B = clamp(pastPos + vec2( cellSize * .25f, -cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 L_T = clamp(pastPos + vec2(-cellSize * .25f,  cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
	vec2 R_T = clamp(pastPos + vec2( cellSize * .25f,  cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));

	//TODO: implement centroid calculations for t
	vec3 colorB = mix(texture(inQuantity, L_B).rgb, texture(inQuantity, R_B).rgb, .5f);
	vec3 colorT = mix(texture(inQuantity, L_T).rgb, texture(inQuantity, R_T).rgb, .5f);

	vec3 phi_hat_n_plus_one = mix(colorB, colorT, 0.5f);
	phi_hat_n_plus_one = normalize(phi_hat_n_plus_one);

  // reverse advection step
  vec2 forwardPos = Tex + (texture(inVelocity, Tex).xy * timeStep);
  forwardPos = clamp(forwardPos, vec2(0.f, 0.f), vec2(1.f, 1.f)); //calulate the bounds shit

  vec2 new_L_B = clamp(forwardPos + vec2(-cellSize * .25f, -cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 new_R_B = clamp(forwardPos + vec2( cellSize * .25f, -cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 new_L_T = clamp(forwardPos + vec2(-cellSize * .25f,  cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));
  vec2 new_R_T = clamp(forwardPos + vec2( cellSize * .25f,  cellSize * .25f), vec2(0.f, 0.f), vec2(1.f, 1.f));

  vec3 new_colorB = mix(texture(inQuantity, new_L_B).rgb, texture(inQuantity, new_R_B).rgb, .5f);
  vec3 new_colorT = mix(texture(inQuantity, new_L_T).rgb, texture(inQuantity, new_R_T).rgb, .5f);

  vec3 phi_hat_n = mix(new_colorB, new_colorT, 0.5f);
  phi_hat_n = normalize(phi_hat_n);

  vec3 output = phi_hat_n_plus_one + (0.5f * (texture(inQuantity, Tex), phi_hat_n))

	FragColor = vec4(output, 1.f);
}