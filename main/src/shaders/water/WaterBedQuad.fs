#version 440 core

in vec2 a_texcoords_out;
uniform sampler2D u_textureSamplerUniform;

out vec4 FragColor;

void main(void)
{
    FragColor = texture(u_textureSamplerUniform, a_texcoords_out);
}


