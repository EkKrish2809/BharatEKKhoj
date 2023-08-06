#version 440 core

in vec4 a_position;
in vec4 a_color;

out vec4 clipSpaceCoords;
out vec3 toCameraVector;
out vec3 lightDirection;
out vec2 a_texcoords_out;
out vec4 a_color_out;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

uniform vec3 u_cameraPosition;
uniform vec3 u_lightPosition;

const float tiling = 4.0;

void main(void)
{
	vec4 worldPosition = u_modelMatrix * vec4(a_position.x, 0.0f, a_position.y, 1.0);
	clipSpaceCoords = u_projectionMatrix * u_viewMatrix * worldPosition;

	a_texcoords_out = vec2(a_position.x/2.0 + 0.5, a_position.y/2.0 + 0.5) * tiling;
	//a_texcoords_out = vec2(a_position.x, a_position.y) * tiling;

	//Calculate camera vector
	toCameraVector = u_cameraPosition - worldPosition.xyz;

	//calculate light direction
	lightDirection = worldPosition.xyz - u_lightPosition;

	gl_Position = clipSpaceCoords;
	a_color_out = a_color;
}



