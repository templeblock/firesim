#version 330 core

uniform sampler2D textureMap;
uniform vec4 color;

in vec2 Tex;
out vec4 FragColor;

void main()
{
	vec3 color = texture(textureMap, Tex).xyz;
	color.x = (color.x + 1.f)/2.f;
	color.y = (color.y + 1.f)/2.f;
	FragColor = vec4(color.xy, 1.f, 1.f);
}