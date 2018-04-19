#version 330 core

uniform sampler2D input_texture;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	FragColor = texture(input_texture, Tex);
}