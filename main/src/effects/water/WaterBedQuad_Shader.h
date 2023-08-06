#pragma once

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"

class WaterBedQuadShader
{
public:
    // Variables
//Water Bed Quad
    GLuint shaderProgramObject_waterBedQuad;

    GLuint modelMatrixUniform_waterBedQuad;
    GLuint viewMatrixUniform_waterBedQuad;
    GLuint projectionMatrixUniform_waterBedQuad;
    GLuint textureSamplerUniform_waterBedQuad;

    bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/water/WaterBedQuad.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/water/WaterBedQuad.fs", FRAGMENT);

        shaderProgramObject_waterBedQuad = glCreateProgram();

        // Step 2
        glAttachShader(shaderProgramObject_waterBedQuad, vertexShaderObject);
        glAttachShader(shaderProgramObject_waterBedQuad, fragmentShaderObject);

        // Pre-Linking Binding Of Vertex Attribute Locations
        glBindAttribLocation(shaderProgramObject_waterBedQuad, MATRIX_ATTRIBUTE_POSITION, "a_position");
        glBindAttribLocation(shaderProgramObject_waterBedQuad, MATRIX_ATTRIBUTE_COLOR, "a_color");

        if (LinkShaderProgramObject(shaderProgramObject_waterBedQuad) == false)
        {
            PrintLog("LinkShaderProgramObject() Failed for waterBedQuad.\n");
            printf("LinkShaderProgramObject() Failed for waterBedQuad.\n");
            return(false);
        }

        // Post-Linking Getting Of Uniform Locations
        modelMatrixUniform_waterBedQuad = glGetUniformLocation(shaderProgramObject_waterBedQuad, "u_modelMatrix");
        viewMatrixUniform_waterBedQuad = glGetUniformLocation(shaderProgramObject_waterBedQuad, "u_viewMatrix");
        projectionMatrixUniform_waterBedQuad = glGetUniformLocation(shaderProgramObject_waterBedQuad, "u_projectionMatrix");
        textureSamplerUniform_waterBedQuad = glGetUniformLocation(shaderProgramObject_waterBedQuad, "u_textureSamplerUniform");

        printf("water bed quad shader\n");
        return true;
    }

    void update(void)
    {
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject_waterBedQuad);
    }
};
