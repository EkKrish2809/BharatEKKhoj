#version 460 core

in vec4 a_position;
in vec3 a_normal;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform float u_scale;
uniform vec3 u_lightPosition;

//out vec4 a_color_out;
out vec3 a_position_out;
out float a_lightIntensity_out;

void main(void)
{
	// vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;
	vec4 eyeCoordinates = a_position;
	a_position_out = vec3(a_position) * u_scale;
	mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);
	vec3 tramsformedNormals = normalize(normalMatrix * (a_normal * -1.0));
	a_lightIntensity_out = dot(normalize(u_lightPosition - vec3(eyeCoordinates)), tramsformedNormals);
	a_lightIntensity_out *= 5.5;
	gl_Position = u_projectionMatrix *u_viewMatrix*u_modelMatrix * a_position;

}
