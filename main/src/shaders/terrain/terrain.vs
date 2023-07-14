#version 460 core
in vec3 a_position;
in vec2 a_texcoord;

out vec2 a_texCoord_out;

void main()
{
    gl_Position = vec4(a_position, 1.0);
    a_texCoord_out = a_texcoord;
}
