#version 330

in vec3 pc_color;

out vec4 color;

void main(){
	color = vec4(pc_color, 1.0f);
};