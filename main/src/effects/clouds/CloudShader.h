#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"

class CloudShader
{
public:
    // Variables
    Camera camera;
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint projectMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;    // model View Projection
    GLuint modelMatrixUniform;   // model View Projection
    GLuint scaleUniform;
    GLuint lightPositionUniform;

    GLuint offsetUniform;
    GLuint skyColorUniform;
    GLuint cloudColorUniform;
    GLuint noiseUniform;

    float lightPos[4] = {0.0f, 0.0f, 4.0f, 1.0f};

    // float skyColor[3] = {194.0f/255.0f, 213.0f/255.0f, 220.0f/255.0f};
    float skyColor[3] = {0.0f, 0.0f, 0.0f};
    // float skyColor[3] = {0.1f, 0.2f, 0.5f};

    // float cloudColor[3] = {0.6f, 0.6f, 0.8f};
    float cloudColor[3] = {194.0f/255.0f, 213.0f/255.0f, 220.0f/255.0f};
    
    GLfloat scaleFactor = 1.0f;
    Bool isScaled = False;

    Bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/clouds/cloud.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/clouds/cloud.fs", FRAGMENT);

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
        scaleUniform = glGetUniformLocation(shaderProgramObject, "u_scale");
        lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");

        offsetUniform = glGetUniformLocation(shaderProgramObject, "u_offset");
        skyColorUniform = glGetUniformLocation(shaderProgramObject, "u_skyColor");
        cloudColorUniform = glGetUniformLocation(shaderProgramObject, "u_cloudColor");
        noiseUniform = glGetUniformLocation(shaderProgramObject, "u_noise");

        return True;
    }

    void update_CloudShaderObjectUniforms(void)
    {
        glUniform3fv(lightPositionUniform, 1, lightPos);
        glUniform1f(scaleUniform, scaleFactor);

        glUniform3fv(skyColorUniform, 1, skyColor);
        glUniform3fv(cloudColorUniform, 1, cloudColor);

        if (isScaled == True)
        {
            scaleFactor = scaleFactor - 0.0004f;

            if (scaleFactor < 1.0f)
                isScaled = False;
        }
        else
        {
            // scaleFactor = scaleFactor + 0.1f;
            scaleFactor = scaleFactor + 0.0004f;
            if (scaleFactor > 1.5f)
                isScaled = True;
        }
    }

    void renderClouds(GLuint texture_noise_cloud, GLuint vao_Sphere, int sphere_vertices, float current_time)
    {
        // code
        mat4 translationMatrix = mat4::identity();
        mat4 rotateMatrix = mat4::identity();
        mat4 scaleMatrix = mat4::identity();

        pushMatrix(modelMatrix);
        {
            pushMatrix(viewMatrix);
            {
                // use shader program obejct
                glUseProgram(shaderProgramObject);

                translationMatrix = mat4::identity();
                modelMatrix = mat4::identity();
                // viewMatrix = mat4::identity();
                // viewMatrix = camera.getViewMatrix();

                translationMatrix = vmath::translate(0.0f, 0.0f, -1000.0f);
                scaleMatrix = vmath::scale(2.0f, 2.0f, 2.0f);
                scaleMatrix = vmath::scale(5000.0f, 5000.0f, 5000.0f);

                modelMatrix = modelMatrix * translationMatrix * scaleMatrix;
                // modelMatrix = translationMatrix * scaleMatrix;

                glUniformMatrix4fv(projectMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
                glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
                glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

                update_CloudShaderObjectUniforms();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_3D, texture_noise_cloud);
                glUniform1i(noiseUniform, 0);

                glBindVertexArray(vao_Sphere);

                glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere_vertices);

                glBindVertexArray(0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_3D, 0);
                // unuse the shader program object
                glUseProgram(0);
            }
            viewMatrix = popMatrix();
        }
        modelMatrix = popMatrix();
    }

    void update(void)
    {
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
