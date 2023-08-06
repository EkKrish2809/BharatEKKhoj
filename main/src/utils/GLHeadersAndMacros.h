#pragma once

// GLEW Header Files
// OpenGL header files
#include <GL/glew.h>
#include <GL/glu.h> // gluPerspective()
#include <GL/gl.h>  // For OpenGL functionality
#include <GL/glx.h> // For Bridging APIs

#include <string.h>

enum
{
    MATRIX_ATTRIBUTE_POSITION = 0,
    MATRIX_ATTRIBUTE_COLOR,
    MATRIX_ATTRIBUTE_NORMAL,
    MATRIX_ATTRIBUTE_TEXTURE0,
	MATRIX_ATTRIBUTE_SEED,
	MATRIX_ATTRIBUTE_VELOCITY
};

enum GLShaderType
{
	VERTEX = 0,
	TESSELLATION_CONTROL,
	TESSELLATION_EVALUATION,
	GEOMETRY,
	FRAGMENT,
	COMPUTE
};


#define SCENE_0_TIMER 0.0
#define SCENE_1_TIMER 16.0
#define SCENE_2_TIMER 26.0
#define SCENE_3_TIMER 36.0
#define SCENE_4_TIMER 46.0
#define SCENE_5_TIMER 56.0
#define SCENE_6_TIMER 66.0




#define SCENE_1 0
#define SCENE_2 1
#define SCENE_3 2
//50.0f // entering into earth [lava terrain]



