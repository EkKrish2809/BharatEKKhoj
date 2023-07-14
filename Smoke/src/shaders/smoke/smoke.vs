#version 460 core

in vec3 a_position;
in vec2 a_texcoord;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

out vec2 a_texcoord_out;

void main(void)
{
    a_texcoord_out = a_texcoord;
    gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position; 
}