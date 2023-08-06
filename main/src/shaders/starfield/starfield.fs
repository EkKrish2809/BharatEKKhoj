#version 460 core

in vec4 starColor;

uniform sampler2D u_starTextureSampler;

layout (location=0)out vec4 FragColor;

void main(void)
{

	// FragColor=texture(u_starTextureSampler,gl_PointCoord);
	//FragColor=starColor;
	FragColor=starColor*texture(u_starTextureSampler,gl_PointCoord);
	
}

