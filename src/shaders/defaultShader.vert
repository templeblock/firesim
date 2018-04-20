#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec3 position;
out vec2 Tex;

void main()
{
	Tex = aTex;
	position = aPos;
	gl_Position = vec4(aPos, 1.0f);
}