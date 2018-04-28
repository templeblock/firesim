#version 330 core

uniform sampler3D input_texture;
uniform float slice;
uniform float cellSize;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	FragColor = texture(input_texture, vec3(Tex, slice * cellSize));
}