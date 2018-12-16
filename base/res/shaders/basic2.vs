#version 330 core

layout (location = 0) in vec2 v;
layout (location = 1) in vec3 pcC;

out vec3 pc_color;

uniform mat4 projection;

void main()
{
	pc_color = pcC;
	gl_Position = projection * vec4(v, 0.0f, 1.0f);
};