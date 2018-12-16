#version 330 core

in vec2 v;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(v, 0.0f, 1.0f);
};