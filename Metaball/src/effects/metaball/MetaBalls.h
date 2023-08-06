#ifndef METABALLS_H
#define METABALLS_H

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"

#include "../../shaders/sunsurface/SunSurface.h"

class MetaBalls
{
public:
    GLuint shaderProgramObject;

    GLuint projectMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;    // model View Projection
    GLuint modelMatrixUniform;   // model View Projection

    GLuint samplerBoxUniform;
    GLuint timeUniform;
    GLuint resolutionUniform;

    GLuint vao, vbo[2];

    SunSurfaceShader sunShader;

    MetaBalls() {}

    Bool initialize(void)
    {
        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/metaballs.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/metaballs.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();

        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "position");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "texcoord");

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject);
        if (bLinkShaderStatus == False)
        {
            return False;
        }

        projectMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        samplerBoxUniform = glGetUniformLocation(shaderProgramObject, "box");
        timeUniform = glGetUniformLocation(shaderProgramObject, "time");
        resolutionUniform = glGetUniformLocation(shaderProgramObject, "resolution");

        float position[] = {
            -1.f, 1.f,
            1.f, -1.f,
            -1.f, -1.f,
            1.f, 1.f,
            1.f, -1.f,
            -1.f, 1.f};

        float texCoord[] = {
            0.f,
            1.f,
            1.f,
            0.f,
            0.f,
            0.f,
            1.f,
            1.f,
            1.f,
            0.f,
            0.f,
            1.f,
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(2, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        printf("In initialize()\n");

        return True;
    }

    void display(void)
    {
        // code
        printf("In display()\n");
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        glUseProgram(shaderProgramObject);

        glUniformMatrix4fv(projectMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

        glUniform1f(timeUniform, ELAPSED_TIME);
        //  glUniform2f(resolutionUniform, 1920.0f, 1080.0f);
        glUniform2f(resolutionUniform, giWindowWidth, giWindowHeight);
        glUniform1i(samplerBoxUniform, 0);
        glActiveTexture(GL_TEXTURE0);
        //  glBindTexture(GL_TEXTURE_2D, sunShader.)

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
        glDisableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindVertexArray(0);

        glUseProgram(0);
    }
};

#endif
