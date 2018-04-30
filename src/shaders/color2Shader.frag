#version 330 core

uniform sampler3D inTexture;

in vec2 Tex;
out vec4 FragColor;

void main()
{
  float temp = texture(inTexture, Tex).x;
  float c1 = 3.7418 * pow(10, -16);
  float c2 = 1.4388 * pow(10, -2);
  float wave_len_r = 0.00000068; // wavelength in meters for red
  float wave_len_g = 0.00000054; // wavelength in meters for green
  float wave_len_b = 0.00000045; // wavelength in meters for blue
  float r = (2 * c1)/(pow(wave_len_r, 5) * (exp(c2/(wave_len_r * temp)) - 1));
  float g = (2 * c1)/(pow(wave_len_g, 5) * (exp(c2/(wave_len_g * temp)) - 1));
  float b = (2 * c1)/(pow(wave_len_b, 5) * (exp(c2/(wave_len_b * temp)) - 1));
  vec3 new_rgb = vec3(0 * r, 0.5 * g, 0.5 * b); // mess around with scaling for temp output range
	FragColor = vec4(new_rgb, 1.f);
}