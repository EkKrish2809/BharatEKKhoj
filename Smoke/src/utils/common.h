#pragma once

// Texture header
#include <SOIL/SOIL.h>

#include "GLHeadersAndMacros.h"
#include "GLLog.h"

#include "../../includes/vmath.h"
#include "../../includes/std_image.h"

using namespace vmath;

#define MODEL_VIEW_MATRIX_STACK 32

extern float objX;
extern float objY;
extern float objZ;
extern float objIncrement;

extern float scaleX;
extern float scaleY;
extern float scaleZ;
extern float scaleIncrement;

extern mat4 perspectiveProjectionMatrix;

extern vec3 eye;
extern vec3 center;
extern vec3 up;

extern mat4 modelMatrix;
extern mat4 viewMatrix;

extern int giWindowWidth;
extern int giWindowHeight;

extern mat4 matrixStack[MODEL_VIEW_MATRIX_STACK];
extern int matrixStackTop;

Bool LoadPNGImage(GLuint *texture, const char *imageFilePath);
Bool LoadGlTexture(GLuint *texture, const char *path);
Bool Load1DPNGImage(GLuint *texture, float *data);

void initializeMatrixStack(void);
void pushMatrix(mat4 matrix);
mat4 popMatrix(void);

// Deg to Rad
float degToRad(float);

// vector subtraction
void Vec3Subtract(vec3& out, const vec3& a, const vec3& b);
