#version 330 core

uniform sampler2D textureMap;
uniform vec4 color;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(textureMap, Tex).xy, 1.f, 1.f);
}