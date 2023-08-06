#include "common.h"

// // Object
// float objX = 0.0;
// float objY = 0.0;
// float objZ = 0.0;
// float objIncrement = 1.0f;

// // Scale
// float scaleX = 0.0;
// float scaleY = 0.0;
// float scaleZ = 0.0;
// float scaleIncrement = 0.01;
// timer



mat4 matrixStack[MODEL_VIEW_MATRIX_STACK];
int matrixStackTop = -1;

Bool LoadPNGImage(GLuint *texture, const char *imageFilePath)
{
    // variable declarations
    int image_width = 0;
    int image_height = 0;
    int numImageComponents = 0;
    unsigned char *image_data = NULL;
    GLenum image_format = 0;

    // code
    glGenTextures(1, texture);
    image_data = stbi_load(imageFilePath, &image_width, &image_height, &numImageComponents, 0);
    if (image_data != NULL)
    {
        if (numImageComponents == 1)
            image_format = GL_RED;
        else if (numImageComponents == 3)
            image_format = GL_RGB;
        else if (numImageComponents == 4)
            image_format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, *texture);

        if (image_format == GL_RGBA)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, image_format, image_width, image_height, 0, image_format, GL_UNSIGNED_BYTE, (void *)image_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image_data);
        return (True);
    }
    return (False);
}

Bool Load1DPNGImage(GLuint *texture, float *data)
{
    // variable declarations
    int image_width = 0;
    int image_height = 0;
    int numImageComponents = 0;
    unsigned char *image_data = NULL;
    GLenum image_format = 0;

    // code
    glGenTextures(1, texture);
    // image_data = stbi_load(imageFilePath, &image_width, &image_height, &numImageComponents, 0);
    // if (image_data != NULL)
    {
        // if (numImageComponents == 1)
        //     image_format = GL_RED;
        // else if (numImageComponents == 3)
        //     image_format = GL_RGB;
        // else if (numImageComponents == 4)
        //     image_format = GL_RGBA;

        glBindTexture(GL_TEXTURE_1D, *texture);

        if (image_format == GL_RGBA)
        {
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        else
        {
            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // glTexImage2D(GL_TEXTURE_2D, 0, image_format, image_width, image_height, 0, image_format, GL_UNSIGNED_BYTE, (void *)image_data);
        
         glTexImage1D( GL_TEXTURE_1D,
                        0,
                        GL_R32F,
                        370,
                        0,
                        GL_RED,
                        GL_FLOAT,
                        (void *)data);
        // stbi_image_free(image_data);
        glGenerateMipmap(GL_TEXTURE_1D);
        return (True);
    }
    return (False);
}

Bool LoadGlTexture(GLuint *texture, const char *path)
{
	// variable declarations
	int width, height;
	unsigned char *imageData = NULL;

	// code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
	if (imageData == NULL)
	{
        printf("SOIL::Failed to load texture\n");
		return False;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// gluBuild2DMipmaps
	glTexImage2D(GL_TEXTURE_2D, 
			0, 
			GL_RGB, 
			width, 
			height, 
			0,
			GL_RGB, 
			GL_UNSIGNED_BYTE, 
			imageData);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	SOIL_free_image_data(imageData);
	return True;

}

void initializeMatrixStack(void)
{
    // code
    matrixStackTop = 0;
    for (int i = matrixStackTop; i < MODEL_VIEW_MATRIX_STACK; i++)
        matrixStack[i] = mat4::identity();
}

void pushMatrix(mat4 matrix)
{
    // Function Prototype
    void uninitialize(void);

    //	Code
    if (matrixStackTop > (MODEL_VIEW_MATRIX_STACK - 1))
    {
        PrintLog("ERROR - EXCEEDED MATRIX STACK LIMIT:\n");
        uninitialize();
    }

    matrixStack[matrixStackTop] = matrix;
    matrixStackTop++;
}

mat4 popMatrix(void)
{
    // function ptototype
    void uninitialize(void);

    // variable declartions
    mat4 matrix;

    // code
    if (matrixStackTop < 0)
    {
        PrintLog("ERROR : MATRIX STACK EMPTY!\n");
        uninitialize();
    }

    matrixStack[matrixStackTop] = mat4::identity();
    matrixStackTop--;

    matrix = matrixStack[matrixStackTop];

    return (matrix);
}

float degToRad(float degrees)
{
    return (degrees * M_PI / 180.0);
}
