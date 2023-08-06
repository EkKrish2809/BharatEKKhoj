#version 460 core

layout (location=0) in vec4 a_position;
layout (location=1) in vec4 a_color;

uniform float fTime;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

float fMinusValue=1000.0f;

out vec4 starColor;

vec4 newPosition;

	float fSize;
	vec4 tempPosition;
	vec4 ptSize;

void main(void)
{

	//local:
	
	
	//code:
	
	starColor=a_color;
	tempPosition=a_position;


	fSize=(2000.0*tempPosition.z*tempPosition.z);

	tempPosition.z+=fTime;
	tempPosition.z=fract(tempPosition.z);
	
	fSize=(20.0f*tempPosition.z*tempPosition.z);

	tempPosition.z=(1000 * tempPosition.z)-1000.0;
	

	newPosition=u_projectionMatrix*u_viewMatrix*u_modelMatrix*tempPosition;

	gl_PointSize=fSize;
	gl_Position=newPosition;

	
}
