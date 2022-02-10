#version 330 core

layout (location = 0) in vec3 p_position_attribute;

void main() {
    gl_Position = vec4(p_position_attribute, 1.0f);
}