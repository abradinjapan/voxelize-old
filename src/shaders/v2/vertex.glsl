#version 330 core

layout (location = 0) in vec3 l_position;
layout (location = 1) in vec2 l_texture_coordinates;

out vec2 pass_texture_coordinates;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(l_position, 1.0);
	pass_texture_coordinates = vec2(l_texture_coordinates.x, l_texture_coordinates.y);
}