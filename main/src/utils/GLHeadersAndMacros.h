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

