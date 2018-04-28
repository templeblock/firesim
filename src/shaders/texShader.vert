#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 model;
uniform mat4 projection;
out vec2 Tex;
out vec4 position;

void main()
{
	Tex = aTex;
	position = model * vec4(aPos, 1.0f);
	gl_Position = vec4(aPos, 1.0f);
}