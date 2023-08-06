#version 450 core
in vec4 a_position;
uniform mat4 u_mvpMatrix;

void main(void)
{
gl_Position = u_mvpMatrix * a_position;
}

