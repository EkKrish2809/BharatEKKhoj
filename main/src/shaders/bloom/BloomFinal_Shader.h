#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"

class BloomFinalShader
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint projectMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;    // model View Projection
    GLuint modelMatrixUniform;   // model View Projection


    GLuint sceneTextureSamplerUniform;
    GLuint blurTextureSamplerUniform;
    GLuint noBloomSceneTextureSamplerUniform;
    GLuint isBloomUniform;
    GLuint exposureUniform;

    Bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/bloom/final.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/bloom/final.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "aPos");
        // Binding Color Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "aTexCoords");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == False)
            return False;

        // post link - getting
        projectMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
        sceneTextureSamplerUniform = glGetUniformLocation(shaderProgramObject, "scene");
        blurTextureSamplerUniform = glGetUniformLocation(shaderProgramObject, "bloomBlur");
        noBloomSceneTextureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_sceneWithoutBloom");
        
        isBloomUniform = glGetUniformLocation(shaderProgramObject, "bloom");
        exposureUniform = glGetUniformLocation(shaderProgramObject, "exposure");

        return True;
    }

    void uninitialize_WoodShaderObject(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};