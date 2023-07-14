#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "../../utils/OpenGL/Framebuffer.h"

class ColorShader
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint projectMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;    // model View Projection
    GLuint modelMatrixUniform;   // model View Projection
    GLuint resolutionUniform;
    GLuint textureSamplerUniform;

    // FRAMEBUFFER
    FrameBuffer *smoke_buffer = NULL;
    GLuint fbo_smokeA;
    GLuint rbo_smokeA;
    GLuint fbo_texture_smokeA;

    GLuint fbo_smokeB;
    GLuint rbo_smokeB;
    GLuint fbo_texture_smokeB;

    GLuint texture_smoke;
    
    Bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smoke.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smoke.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding texture Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == False)
            return False;
            
        // post link - getting
        projectMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        // smoke fbo
        Bool bResult = smoke_buffer->CreateFBO(1920, 1080, fbo_smokeA, rbo_smokeA, fbo_texture_smokeA);
        if (!bResult)
        {
            PrintLog("fbo_smokeA : CreateFBO Failed\n");
            return False;
        }

        bResult = smoke_buffer->CreateFBO(1920, 1080, fbo_smokeB, rbo_smokeB, fbo_texture_smokeB);
        if (!bResult)
        {
            PrintLog("fbo_smokeB : CreateFBO Failed\n");
            return False;
        }

        return True;
    }

    void display(void)
    {
        // code

        // 1. Pass A through your shader, render onto B.
        {
            glUseProgram(shaderProgramObject);

            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(projectMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

            glUniform2f(resolutionUniform, (GLfloat)giWindowWidth, (GLfloat)giWindowHeight);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_smoke);
            glUniform1i(textureSamplerUniform, 0);

            glUseProgram(0);
        }

        // 2. Render B to the screen.
        // 3. Pass B through shader, render onto A.
        // 4. Render A to your screen.
        // 5. Repeat 1.
    }

    void update(void)
    {
    }

    void uninitialize(void)
    {
    }
};
