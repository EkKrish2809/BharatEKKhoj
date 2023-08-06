// TextureSimple.h
#pragma once

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
// #include "./../utils/OGLTime.h"

#include <cstdlib>
#include <cmath>

#define STAR_NUMS 12

#define MAX_STAR_NUMS 201

// Random number generator
static unsigned int seed = 0x13371337;

static inline float random_float()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

const int newStarElements = 15000;
const int newStarVertices = 5000;

GLfloat newStarPos2[newStarElements];
GLfloat newStarCol2[newStarElements];

GLuint Texture_Star;

class Starfield
{
public:
    // Variables
    GLuint vao_newStars_One;
    GLuint vbo_newStarPositions_1;
    GLuint vbo_newStarColors_1;

    GLuint vao_newStars_Two;
    GLuint vbo_newStarPositions_2;
    GLuint vbo_newStarColors_2;

    GLuint shaderProgramObject;
    GLuint projectionMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;       // model View Projection
    GLuint modelMatrixUniform;      // model View Projection

    GLuint fTimeValue_Uniform;
    GLfloat fTime = 100.0f;

    GLuint textureSamplerUniform;

    GLfloat *fStarArray = NULL;

    /* Function Definitions */
    Bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/starfield/starfield.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/starfield/starfield.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding Tex Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_COLOR, "a_color"); // ANDHAAAR

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == False)
            return False;

        // post link - getting
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
        // fTimeValue_Uniform
        // fTimeValue_Uniform = glGetUniformLocation(shaderProgramObject, "fTime");

        const GLfloat pointPosition[] =
            {
                1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, -1.0f,
                -1.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f

            };

        const GLfloat pointColor[] =
            {
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f};

        // #----NEW -POINT STRIPE 1 -----#
        glGenVertexArrays(1, &vao_newStars_One);
        glBindVertexArray(vao_newStars_One);
        {
            // VBO For Position :
            glGenBuffers(1, &vbo_newStarPositions_1);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_newStarPositions_1);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(pointPosition), pointPosition, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0); // f:

            // VBO  Color:
            glGenBuffers(1, &vbo_newStarColors_1);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_newStarColors_1);
            {
                glVertexAttrib3f(MATRIX_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_COLOR);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0); // f:
        }
        glBindVertexArray(0); // A] 6 :

        int index = 0;
        for (int i = 0; i < newStarVertices; i++)
        {
            index = i * 3;

            newStarPos2[index] = (random_float() * 2.0f - 1.0f) * 500.0f;
            newStarPos2[index + 1] = (random_float() * 2.0f - 1.0f) * 500.0f;
            newStarPos2[index + 2] = random_float();
            // newStarCol2[index] = 0.8f + random_float() * 1.0f;//R
            // newStarCol2[index+1] = 0.8f + random_float() * 1.0f;
            // newStarCol2[index+2] = 0.8f + random_float() * 1.0f;//B

            newStarCol2[index] = 1.0f; // R
            newStarCol2[index + 1] = 1.0f;
            newStarCol2[index + 2] = 1.0f; // B

            // printf("\n $$ newStarPos2[%d], newStarPos2[%d], newStarPos2[%d] = %f , %f , %f ",index,index+1,index+2,newStarPos2[index],newStarPos2[index+1],newStarPos2[index+2]);
            // printf("\n $$ newStarCol2[%d], newStarCol2[%d], newStarCol2[%d] = %f , %f , %f ",index,index+1,index+2,newStarCol2[index],newStarCol2[index+1],newStarCol2[index+2]);
        }

        printf("\n # # sizeof(newStarPos2) = %ld", sizeof(newStarPos2));

        // #----NEW -POINT STRIPE 2 -----#
        glGenVertexArrays(1, &vao_newStars_Two);
        glBindVertexArray(vao_newStars_Two);
        {
            // VBO For Position :
            glGenBuffers(1, &vbo_newStarPositions_2);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_newStarPositions_2);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(newStarPos2), newStarPos2, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // VBO  Color:
            glGenBuffers(1, &vbo_newStarColors_2);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_newStarColors_2);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(newStarCol2), newStarCol2, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_COLOR);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        // initTimer();

        glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

        if (LoadGlTexture(&Texture_Star, "./assets/textures/starfield/starTexture1.bmp") == False)
        {
            // error:
            PrintLog("\nStar LoadGlTexture() Failed. \n");
            return False;
        }

        return True;
    }

    void renderStarField_One(double currentTime)
    {
        // local:
        static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
        static const GLfloat blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
        static const GLfloat one[] = {1.0f};

        // float fTime=(float)currentTime;
        // fTime*=0.1f;
        // fTime-=floor(fTime);
        static float fTime = 0.0f;

        fTime += 0.001f;

        glClearBufferfv(GL_COLOR, 0, black);
        glClearBufferfv(GL_DEPTH, 0, one);

        glEnable(GL_PROGRAM_POINT_SIZE);

        glUseProgram(shaderProgramObject);

        mat4 modelMatrix = mat4::identity();
        mat4 viewMatrix = mat4::identity();
        mat4 translationMatrix = mat4::identity();

        translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
        // #---VIMP Matrix Multiplication---#--T-S-R--#
        modelMatrix = translationMatrix;
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glUniform1f(fTimeValue_Uniform, fTime);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // Code Here :
        glActiveTexture(GL_TEXTURE0);               // #1st VIMP
        glBindTexture(GL_TEXTURE_2D, Texture_Star); // # 3rd VIMP
        glUniform1i(textureSamplerUniform, 0);
        glBindVertexArray(vao_newStars_One);
        glDrawArrays(GL_POINTS, 0, 9);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }

    void renderStarField_Two(double currentTime)
    {
        // local:
        static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 0.0f};
        static const GLfloat blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
        static const GLfloat one[] = {1.0f};

        /*             float fTime=(float)currentTime;

                    fTime*=0.1f;

                    fTime-=floor(fTime); */

        static float fTime = 0.0f;

        fTime += 0.0001f;

        glClearBufferfv(GL_COLOR,0,black);
        glClearBufferfv(GL_DEPTH,0,one);

        glUseProgram(shaderProgramObject);

        // mat4 modelMatrix = mat4::identity();
        // mat4 viewMatrix = mat4::identity();
        mat4 translationMatrix = mat4::identity();

        translationMatrix = vmath::translate(0.0f, 0.0f, -9.0f);
        // #---VIMP Matrix Multiplication---#--T-S-R--#
        modelMatrix = translationMatrix;
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, mat4::identity());
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, mat4::identity());
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glUniform1f(fTimeValue_Uniform, fTime);
        // glUniform1f(fTimeValue_Uniform, currentTime);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // Code Here :
        glActiveTexture(GL_TEXTURE0);               // #1st VIMP
        glBindTexture(GL_TEXTURE_2D, Texture_Star); // # 3rd VIMP
        glUniform1i(textureSamplerUniform, 0);
        glBindVertexArray(vao_newStars_Two);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, newStarVertices);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_BLEND);

        glUseProgram(0);
    }

    void updateTime(void)
    {
        // code:
        fTime -= 0.3f;
        // if(fTime>=100.0f)
        // {
        //     fTime-=100.0f;
        // }
    }

    void uninitialize(void)
    {
        // delete VBO  :
        if (vbo_newStarColors_2)
        {
            glDeleteBuffers(1, &vbo_newStarColors_2);
            vbo_newStarColors_2 = 0;
        }
        if (vbo_newStarPositions_2)
        {
            glDeleteBuffers(1, &vbo_newStarPositions_2);
            vbo_newStarPositions_2 = 0;
        }

        // delete VAO
        if (vao_newStars_Two)
        {
            glDeleteVertexArrays(1, &vao_newStars_Two);
            vao_newStars_Two = 0;
        }

        // delete VBO  :
        if (vbo_newStarColors_1)
        {
            glDeleteBuffers(1, &vbo_newStarColors_1);
            vbo_newStarColors_1 = 0;
        }
        if (vbo_newStarPositions_1)
        {
            glDeleteBuffers(1, &vbo_newStarPositions_1);
            vbo_newStarPositions_1 = 0;
        }

        // delete VAO
        if (vao_newStars_One)
        {
            glDeleteVertexArrays(1, &vao_newStars_One);
            vao_newStars_One = 0;
        }

        UninitializeShaders(shaderProgramObject);
    }
};
