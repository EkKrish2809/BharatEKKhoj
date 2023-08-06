// TextureSimple.h
#pragma once

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"

class SunSurfaceShader
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint projectMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;    // model View Projection
    GLuint modelMatrixUniform;   // model View Projection

    GLuint lightPosUniform;
    GLuint scaleUniform;
    GLuint Color1Uniform;
    GLuint Color2Uniform;

    GLuint textureSamplerUniform;

    float lightPos[4] = {0.0f, 0.0f, 4.0f, 1.0f};

    float Color1[3] = {0.8f, 0.7f, 0.0f};
    float Color2[3] = {0.6f, 0.1f, 0.0f};

    GLfloat scaleFactor = 2.0f;
    Bool isScaled = False;

    Bool initialize_SunSurafaceShaderObject(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/sunsurface/sunsurface.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/sunsurface/sunsurface.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding Color Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_NORMAL, "a_normal");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == False)
            return False;
        // post link - getting
        projectMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        lightPosUniform = glGetUniformLocation(shaderProgramObject, "LightPos");
        scaleUniform = glGetUniformLocation(shaderProgramObject, "NoiseScale");

        textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");

        Color1Uniform = glGetUniformLocation(shaderProgramObject, "Color1"); // Sky related uniform
        Color2Uniform = glGetUniformLocation(shaderProgramObject, "Color2"); // Sky related uniform
        return True;
    }

    void update_SunSurfaceObjectUniforms(void)
    {
        glUniform3fv(lightPosUniform, 1, lightPos);
        glUniform1f(scaleUniform, scaleFactor);

        glUniform3fv(Color1Uniform, 1, Color1);
        glUniform3fv(Color2Uniform, 1, Color2);

        if (isScaled == True)
        {
            scaleFactor = scaleFactor - 0.001f;

            if (scaleFactor < 1.0f)
                isScaled = False;
        }
        else
        {
            // scaleFactor = scaleFactor + 0.1f;
            scaleFactor = scaleFactor + 0.001f;
            if (scaleFactor > 2.0f)
                isScaled = True;
        }
    }

    void uninitialize_WoodShaderObject(void)
    {
    }
};
