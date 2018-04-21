#version 330 core

uniform sampler2D inTex;
uniform vec4 offset;
uniform float scalar;

in vec2 Tex;

out vec4 FragColor;

void main()
{
	vec2 newTex = Tex + offset.xy;
	vec3 res = texture(inTex, newTex).xyz * scalar;
	FragColor = vec4(res, 1.f);
}