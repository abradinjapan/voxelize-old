#version 330 core

out vec4 pass_fragment_color;

in vec3 pass_color;
in vec2 pass_texture_coordinates;

uniform sampler2D u_texture_1;

void main() {
	pass_fragment_color = texture(u_texture_1, pass_texture_coordinates);
	//pass_fragment_color = vec4(0.0, 1.0, 0.0, 1.0);
}