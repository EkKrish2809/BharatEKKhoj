#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"

class BlurShader
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint textureSamplerUniform;
    GLuint isHorizontalUniform;

    Bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/bloom/blurLearnOpenGL.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/bloom/blurLearnOpenGL.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding Color Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == False)
            return False;
            
        // post link - getting
        textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
        isHorizontalUniform = glGetUniformLocation(shaderProgramObject, "horizontal");

        return True;
    }

    void uninitialize_WoodShaderObject(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};