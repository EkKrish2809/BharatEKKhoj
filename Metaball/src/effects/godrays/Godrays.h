#pragma once

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "../../utils/OpenGL/DDSLoader.h"

class Fire
{

public:

    // Member Variables
    GLuint vao_fire;
    GLuint vbo_position_fire;
    GLuint vbo_texcoord_fire;

    GLuint shaderProgramObject_fire;

    GLuint modelMatrixUniform_fire;
    GLuint viewMatrixUniform_fire;
    GLuint projectionMatrixUniform_fire;

    GLuint fireSamplerUniform_fire, noiseSamplerUniform_fire, alphaSamplerUniform_fire;
    GLuint frameTimeUniform_fire, scrollSpeedUniform_fire, scaleUniform_fire, distortion1Uniform_fire, distortion2Uniform_fire, distortion3Uniform_fire, distortionScaleUniform_fire, distortionBiasUniform_fire;

    GLuint texture_fire, texture_noise_fire, texture_alpha_fire;

    // Fire Texture Uniforms
    float frameTime = 0.0f;
    vec3 scrollSpeeds = vec3(1.3f, 2.1f, 2.3f); 
    vec3 scales = vec3(1.0f, 2.0f, 3.0f);
    vec2 distortion1 = vec2(0.1f, 0.2f);
    vec2 distortion2 = vec2(0.1f, 0.3f);
    vec2 distortion3 = vec2(0.1f, 0.1f);
    float distortionScale = 0.8f;
    float distortionBias = 0.5f;

    imageData *ddsImage_fire = NULL;

    // Member Functions
    Bool initializeFire(void)
    {
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/fire/fire.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/fire/fire.fs", FRAGMENT);
    
        shaderProgramObject_fire = glCreateProgram();
        
        glAttachShader(shaderProgramObject_fire, vertexShaderObject);
        glAttachShader(shaderProgramObject_fire, fragmentShaderObject);

        glBindAttribLocation(shaderProgramObject_fire, MATRIX_ATTRIBUTE_POSITION, "a_position");
        glBindAttribLocation(shaderProgramObject_fire, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_fire);

        if (bLinkShaderStatus == False)
        {
            return False;
        }

        modelMatrixUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_modelMatrix");
        viewMatrixUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_viewMatrix");
        projectionMatrixUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_projectionMatrix");
        fireSamplerUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_fireSampler");
        noiseSamplerUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_noiseSampler");
        alphaSamplerUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_alphaSampler");
        frameTimeUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_frameTime");
        scrollSpeedUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_scrollSpeeds");
        scaleUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_scales");
        distortion1Uniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_distortion1");
        distortion2Uniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_distortion2");
        distortion3Uniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_distortion3");
        distortionBiasUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_distortionBias");
        distortionScaleUniform_fire = glGetUniformLocation(shaderProgramObject_fire, "u_distortionScale");

        // Declaration of Vertex Data Arrays
        const GLfloat position[] =
        {
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };

        const GLfloat texcoord[] =
        {
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };
        

        // vao_fire and VBO Related Code
        // --------------------------------------------------------------------------------------------------
        glGenVertexArrays(1, &vao_fire);
        glBindVertexArray(vao_fire);

        // VBO For Position
        glGenBuffers(1, &vbo_position_fire);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_fire);
        glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // VBO For Texcoord
        glGenBuffers(1, &vbo_texcoord_fire);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_fire);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Unbinding of vao_fire
        glBindVertexArray(0);
        // --------------------------------------------------------------------------------------------------

        if (LoadPNGImage(&texture_fire, "./assets/textures/fire/fire.png") == False)
        {
            printf("\nFailed To Load DDS Texture : Fire !!! \n");
            PrintLog("\nFailed To Load DDS Texture : Fire !!! \n");
            return False;
        }

        
        if (LoadPNGImage(&texture_noise_fire, "./assets/textures/fire/noise.png") == False)
        {
            printf("\nFailed To Load DDS Texture : Noise !!! \n");
            PrintLog("\nFailed To Load DDS Texture : Noise !!! \n");
            return False;
        }
        
        if (LoadPNGImage(&texture_alpha_fire, "./assets/textures/fire/alpha.png") == False)
        {
            printf("\nFailed To Load DDS Texture : Alpha !!! \n");
            PrintLog("\nFailed To Load DDS Texture : Alpha !!! \n");
            return False;
        }

        if (glIsEnabled(GL_TEXTURE_2D) == GL_FALSE)
        {
            // Enabling the texture
            glEnable(GL_TEXTURE_2D);
        }

        if (glIsEnabled(GL_BLEND) == GL_FALSE)
        {
            // Enable Blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        return True;

    }

    void renderFire(void)
    {
        glUseProgram(shaderProgramObject_fire);

        // Transformations
        mat4 translationMatrix = mat4::identity();

        translationMatrix = vmath::translate(0.0f, 0.0f, 4.0f);

        modelMatrix = modelMatrix * translationMatrix;

        glUniformMatrix4fv(modelMatrixUniform_fire, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform_fire, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform_fire, 1, GL_FALSE, perspectiveProjectionMatrix);

        // Fire Uniforms
        glUniform1f(frameTimeUniform_fire, frameTime);
        glUniform3fv(scrollSpeedUniform_fire, 1, scrollSpeeds);
        glUniform3fv(scaleUniform_fire, 1, scales);
        glUniform2fv(distortion1Uniform_fire, 1, distortion1);
        glUniform2fv(distortion2Uniform_fire, 1, distortion2);
        glUniform2fv(distortion3Uniform_fire, 1, distortion3);
        glUniform1f(distortionScaleUniform_fire, distortionScale);
        glUniform1f(distortionBiasUniform_fire, distortionBias);

        // Multi-texturing
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(fireSamplerUniform_fire, 0);
        glBindTexture(GL_TEXTURE_2D, texture_fire);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(noiseSamplerUniform_fire, 1);
        glBindTexture(GL_TEXTURE_2D, texture_noise_fire);

        glActiveTexture(GL_TEXTURE2);
        glUniform1i(alphaSamplerUniform_fire, 2);
        glBindTexture(GL_TEXTURE_2D, texture_alpha_fire);

        glBindVertexArray(vao_fire);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }

    void updateFire(void)
    {
        frameTime = frameTime + 0.01f;
        if (frameTime > 1000.0f)
            frameTime = 0.0f;
    }

    void uninitializeFire(void)
    {
        if (ddsImage_fire)
        {
            free(ddsImage_fire);
            ddsImage_fire = NULL;
        }

        if (texture_alpha_fire)
        {
            glDeleteTextures(1, &texture_alpha_fire);
            texture_alpha_fire = 0;
        }

        if (texture_noise_fire)
        {
            glDeleteTextures(1, &texture_noise_fire);
            texture_noise_fire = 0;
        }

        if (texture_fire)
        {
            glDeleteTextures(1, &texture_fire);
            texture_fire = 0;
        }

        if (vbo_texcoord_fire)
        {
            glDeleteBuffers(1, &vbo_texcoord_fire);
            vbo_texcoord_fire = 0;
        }

        if (vbo_position_fire)
        {
            glDeleteBuffers(1, &vbo_position_fire);
            vbo_position_fire = 0;
        }

        if (vao_fire)
        {
            glDeleteVertexArrays(1, &vao_fire);
            vao_fire = 0;
        }

        UninitializeShaders(shaderProgramObject_fire);
    }

};
