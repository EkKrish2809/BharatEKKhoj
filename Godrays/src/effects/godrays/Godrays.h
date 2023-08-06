#ifndef _GODRAYS_H_
#define _GODRAYS_H_

#include "../../utils/common.h"
#include "../Color_Shader.h"
#include "../../../includes/sphere.h"
#include "../../utils/BezierCamera.h"
// #include "../../utils/model_using_assim/model.h"

#define OFF_SCREEN_RENDER_RATIO 2

#define FBO_WIDTH_GODRAYS 1920
#define FBO_HEIGHT_GODRAYS 1080

class Godrays
{
public:
    GLuint shaderProgramObject;

    // color shader
    ColorShader *colorShader = NULL;

    GLuint fbo_UserMap; // FRAME BUFFER OBJECT
    GLuint rbo_UserMap; // RENDER BUFFER OBJECT
    GLuint fbo_texture_UserMap;

    GLuint fbo_ColorMap; // FRAME BUFFER OBJECT
    GLuint rbo_ColorMap; // RENDER BUFFER OBJECT
    GLuint fbo_texture_ColorMap;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    GLuint uniformLightLocation[2];
    GLuint uniformExposure;
    GLuint uniformDecay;
    GLuint uniformDensity;
    GLuint uniformWeight;
    GLuint uniformIllumination;

    GLuint textureSampler;
    GLuint textureSampler_ColorMapSampler;

    float LightX;
    float LightY;
    // float Exposure = 0.0034f;
    // float Decay = 1.0f;
    // float Density = 0.84f;
    // float Weight = 5.65f;

    float Exposure = 0.2f;
    float Decay = 0.97815f;
    float Density = 0.926f;
    float Weight = 0.98767f;

    float texture_screencopy;

    vmath::vec4 lightPosition = vmath::vec4(-2.0f + 14.0f, -31.0f, -5.0 - 3.0f, 1.0f);

    GLuint textureId;
    GLuint texture_ColorMapSampler;

    GLuint vao_quad;
    GLuint vbo_quad_position;
    GLuint vbo_quad_texcoord;

    GLuint vao_sphere;
    GLuint vbo_sphere_position;
    GLuint vbo_sphere_texcoord;

    Sphere *sphere = NULL;

    int camInc = 0;
    bool isCamInc1 = false;
    bool isCamInc2 = false;
    bool isCamInc3 = false;
    int numberOfLights = 1;

    float lightAngles[12] =
        {
            -0.07f + 0.250000f + 0.240000f, 1.219f + 0.010000f - 0.1f,
            0.75f + -0.130000f - 0.1f, 1.219f - 0.09f,
            -0.07f + 0.250000f + 0.340000f, 1.219f + 0.010000f - 0.1f,
            0.75f + -0.130000f - 0.1f, 1.219f - 0.09f,
            -0.07f + 0.250000f + 0.570000f, 1.219f + 0.010000f - 0.1f,
            0.75f + -0.130000f - 0.1f, 1.219 - 0.09f};

    float lightPositions[3] =
        {0.0f, 0.0f, 0.0f};

    GLuint web_1;
    GLuint web_2;
    GLuint web_3;
    GLuint web_4;
    GLuint web_full;

    GLfloat alpha_web1 = 0.7f;
    GLfloat alpha_web2 = 0.4f;
    GLfloat alpha_web3 = 0.4f;
    GLfloat alpha_web4 = 0.4f;
    GLfloat alpha_web_full = 0.0f; // SET THIS TO 4.0f TO REVERT THE CHANGE

    Bool initialize()
    {
        printf("starting Godrays initialization\n");
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/godrays/godrays.vs", VERTEX);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/godrays/godrays.fs", FRAGMENT);
        printf("starting Godrays 1\n");
        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);
        printf("starting Godrays 2\n");

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "texcoord");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);
        if (bShaderLinkStatus == False)
            return False;
        printf("starting Godrays 3\n");

        // post link - getting
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        uniformExposure = glGetUniformLocation(shaderProgramObject, "exposure");
        uniformDecay = glGetUniformLocation(shaderProgramObject, "decay");
        uniformDensity = glGetUniformLocation(shaderProgramObject, "density");
        uniformWeight = glGetUniformLocation(shaderProgramObject, "weight");
        uniformIllumination = glGetUniformLocation(shaderProgramObject, "u_illuminationDecay");
        textureSampler = glGetUniformLocation(shaderProgramObject, "UserMapSampler");
        textureSampler_ColorMapSampler = glGetUniformLocation(shaderProgramObject, "ColorMapSampler");
        uniformLightLocation[0] = glGetUniformLocation(shaderProgramObject, "lightPositionOnScreen[0]");
        uniformLightLocation[1] = glGetUniformLocation(shaderProgramObject, "lightPositionOnScreen[1]");

        GLfloat position[] = {
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f};

        GLfloat coord[] = {
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f};

        sphere = new Sphere(1.2f, 100, 100);

        glGenVertexArrays(1, &vao_quad);
        glBindVertexArray(vao_quad);
        {
            glGenBuffers(1, &vbo_quad_position);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_position);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &vbo_quad_texcoord);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_quad_texcoord);
            {
                glBufferData(GL_ARRAY_BUFFER, sizeof(coord), coord, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        glGenVertexArrays(1, &vao_sphere);
        glBindVertexArray(vao_sphere);
        {
            glGenBuffers(1, &vbo_sphere_position);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
            {
                glBufferData(GL_ARRAY_BUFFER, sphere->getNumberOfSphereVertices() * sizeof(float), sphere->getSphereVertex(), GL_STATIC_DRAW);
                // glBufferData(GL_ARRAY_BUFFER, cone->getNumberOfConeVertices() * sizeof(float), cone->getConeVertex(), GL_STATIC_DRAW);
                // glBufferData(GL_ARRAY_BUFFER, cylinder->vertices->size * sizeof(float), cylinder->vertices->p_arr, GL_STATIC_DRAW);
                glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);

        printf("starting Godrays 4\n");

        bool bResult = CreateFBO(FBO_WIDTH_GODRAYS, FBO_HEIGHT_GODRAYS, fbo_ColorMap, rbo_ColorMap, fbo_texture_ColorMap);
        if (!bResult)
        {
            printf("fbo_ColorMap : CreateFBO Failed\n");
        }

        bResult = CreateFBO(FBO_WIDTH_GODRAYS, FBO_HEIGHT_GODRAYS, fbo_UserMap, rbo_UserMap, fbo_texture_UserMap);
        if (!bResult)
        {
            printf("fbo_UserMap : CreateFBO Failed\n");
        }

        printf("starting Godrays 5\n");

        colorShader = new ColorShader();
        colorShader->initialize();

        return True;
    }

    bool CreateFBO(GLint textureWidth, GLint textureHeight, GLuint &fbo, GLuint &rbo, GLuint &fbo_texture)
    {

        int maxRenderbufferSize;

        // CODE
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

        // CHECK AVAILABLE BUFFER SIZE
        if (maxRenderbufferSize < textureWidth || maxRenderbufferSize < textureHeight)
        {
            PrintLog("UnSufficient Render Buffer Size...\n");
            return (false);
        }

        // CREATE FRAME BUFFER OBJECT
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // CREATE RENDER BUFFER OBJECT
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);

        // WHERE TO KEEP THIS RENDER BUFFER AND WHAT WILL BE THE FORMAT OF RENDER BUFFER
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16, // NOT RELATED TO DEPTH - BUT 16 MACRO SIZE THIS ONE HENCE USED HERE
            textureWidth,
            textureHeight);

        // CREATE EMPTY TEXTURE
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            textureWidth,
            textureHeight,
            0,
            GL_RGB,
            GL_UNSIGNED_SHORT_5_6_5,
            NULL // NULL - EMPTY TEXTURE
        );

        // GIVE ABOVE TEXTURE TO FBO
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            fbo_texture,
            0 // MIPMAP LEVEL
        );

        // GIVE RENDER BUFFER TO FBO
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            rbo //
        );

        // CHECK WATHER FBO IS CREATED SUCCESSFULLY OR NOT
        GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (result != GL_FRAMEBUFFER_COMPLETE)
        {
            PrintLog("FRAME BUFFER IS NOT COMPLETE...\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // IMPLICITEL UNBINDS THE RBO AND TEXTURE BUFFER OBJECT

        return true;
    }

    void getLightScreenCoord()
    {

        // double modelView[16];
        // double projection[16];
        // GLint viewport[4];
        // double depthRange[2];

        // glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
        // glGetDoublev(GL_PROJECTION_MATRIX, projection);
        // glGetIntegerv(GL_VIEWPORT, viewport);
        // glGetDoublev(GL_DEPTH_RANGE, depthRange);

        // GLdouble winX = 0;
        // GLdouble winY = 0;
        // GLdouble winZ = 0;

        // gluProject(lightPosition[0],
        //            lightPosition[1],
        //            lightPosition[2],
        //            modelView,
        //            projection,
        //            viewport,
        //            &winX,
        //            &winY,
        //            &winZ);

        // LightX = winX / ((float)giWindowWidth / OFF_SCREEN_RENDER_RATIO);
        // LightY = winY / ((float)giWindowHeight / OFF_SCREEN_RENDER_RATIO);

        // Get the matrices and viewport
        double modelView[16];
        double projection[16];
        double viewport[4];
        double depthRange[2];

        glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetDoublev(GL_VIEWPORT, viewport);
        glGetDoublev(GL_DEPTH_RANGE, depthRange);

        // Compose the matrices into a single row-major transformation
        vmath::vec4 T[4];
        int r, c, i;
        for (r = 0; r < 4; ++r)
        {
            for (c = 0; c < 4; ++c)
            {
                T[r][c] = 0;
                for (i = 0; i < 4; ++i)
                {
                    // OpenGL matrices are column major
                    T[r][c] += projection[r + i * 4] * modelView[i + c * 4];
                }
            }
        }

        // Transform the vertex
        vmath::vec4 result;
        for (r = 0; r < 4; ++r)
        {
            result[r] = vmath::dot(T[r], lightPosition); // T[r].dot(v);
        }

        // Homogeneous divide
        const double rhw = 1 / result[3];
        LightX = (1 + result[0] * rhw) * viewport[2] / 2 + viewport[0];
        LightY = (1 - result[1] * rhw) * viewport[3] / 2 + viewport[1];
        // return vmath::vec4(
        //     (1 + result[0] * rhw) * viewport[2] / 2 + viewport[0],
        //     (1 - result[1] * rhw) * viewport[3] / 2 + viewport[1],
        //     (result[2] * rhw) * (depthRange[1] - depthRange[0]) + depthRange[0],
        //     rhw);

        LightX = (1 + result[0] * rhw) * viewport[2] / 2 + viewport[0] / ((float)giWindowWidth / OFF_SCREEN_RENDER_RATIO);
        LightY = (1 - result[1] * rhw) * viewport[3] / 2 + viewport[1] / ((float)giWindowHeight / OFF_SCREEN_RENDER_RATIO);
        // PrintLog("LightX: %f ,LightY: %f \n", LightX, LightY);
    }

    void update()
    {
    }

    void display(GLfloat illuminator)
    {
        // printf("Display 1\n");
        // float Exposure = 0.2f;
        float Decay = 0.97815f + 0.02;
        // float Density = 0.926f + objY;
        // float Weight = 0.98767f + objX;

        mat4 translationMatrix = mat4::identity();
        mat4 rotateMatrix = mat4::identity();
        mat4 rotateMatrixX = mat4::identity();
        mat4 rotateMatrixY = mat4::identity();
        mat4 scaleMatrix = mat4::identity();

        // Draw occuding source black with light
        // printf("Display 2\n");
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_UserMap);
        {
            getLightScreenCoord();

            glEnable(GL_TEXTURE_2D);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glViewport(0, 0, FBO_WIDTH_GODRAYS, FBO_HEIGHT_GODRAYS);

            perspectiveProjectionMatrix = vmath::perspective(
                45.0f,
                (GLfloat)FBO_WIDTH_GODRAYS / (GLfloat)FBO_HEIGHT_GODRAYS,
                0.1f,
                1000.0f);

            // glViewport(0, 0, giWindowWidth / 2, giWindowHeight / 2);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            pushMatrix(modelMatrix);
            {
                translationMatrix = mat4::identity();
                translationMatrix = vmath::translate(0.0f, 0.0f, -10.0f);
                // scaleMatrix = vmath::scale(3.0f, 4.0f, 3.0f);
                // scaleMatrix = vmath::scale(1.0f, 2.0f, 1.0f);
                modelMatrix = modelMatrix * translationMatrix * scaleMatrix;
                glVertexAttrib3f(MATRIX_ATTRIBUTE_COLOR, 1.0f, 0.7f, 0.0f);
                drawSphere();
            }
            modelMatrix = popMatrix();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Another
        // printf("Display 3\n");
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_ColorMap);
        {
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            getLightScreenCoord();

            glEnable(GL_TEXTURE_2D);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glViewport(0, 0, FBO_WIDTH_GODRAYS, FBO_HEIGHT_GODRAYS);
            perspectiveProjectionMatrix = vmath::perspective(
                45.0f,
                (GLfloat)FBO_WIDTH_GODRAYS / (GLfloat)FBO_HEIGHT_GODRAYS,
                0.1f,
                1000.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            {
                pushMatrix(modelMatrix);
                {
                    glUseProgram(colorShader->shaderProgramObject);

                    pushMatrix(modelMatrix);
                    {
                        translationMatrix = mat4::identity();
                        scaleMatrix = mat4::identity();
                        rotateMatrix = mat4::identity();
                        rotateMatrixX = mat4::identity();
                        rotateMatrixY = mat4::identity();

                        rotateMatrixX = vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f);
                        rotateMatrixY = vmath::rotate(0.0f, 0.0f, 1.0f, 0.0f);
                        rotateMatrix = rotateMatrixX * rotateMatrixY;

                        // scaleMatrix = vmath::scale(1.0f, 1.2f, 1.0f);
                        modelMatrix = modelMatrix * translationMatrix * rotateMatrix * scaleMatrix;

                        glVertexAttrib3f(MATRIX_ATTRIBUTE_COLOR, 1.0f, 1.0f, 1.0f);
                        glUniformMatrix4fv(colorShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
                        glUniformMatrix4fv(colorShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
                        glUniformMatrix4fv(colorShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

                        glBindVertexArray(vao_sphere);
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere->getNumberOfSphereVertices());
                        glBindVertexArray(0);
                    }
                    modelMatrix = popMatrix();

                    // pushMatrix(modelMatrix);
                    // {
                    //     translationMatrix = mat4::identity();
                    //     translationMatrix = vmath::translate(0.0f, 0.0f, -10.0f);
                    //     // scaleMatrix = vmath::scale(3.0f, 4.0f, 3.0f);
                    //     // scaleMatrix = vmath::scale(1.0f, 2.0f, 1.0f);
                    //     modelMatrix = modelMatrix * translationMatrix * scaleMatrix;
                    //     glVertexAttrib3f(MATRIX_ATTRIBUTE_COLOR, 0.0f, 0.0f, 0.0f);

                    //     glBindVertexArray(vao_sphere);
                    //     glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere->getNumberOfSphereVertices());
                    //     glBindVertexArray(0);
                    // }
                    // modelMatrix = popMatrix();

                    glUseProgram(0);
                }
                modelMatrix = popMatrix();
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Final scene
        // printf("Display 4\n");
        glUseProgram(shaderProgramObject);
        {

            // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, giWindowWidth, giWindowHeight);
            perspectiveProjectionMatrix = vmath::perspective(
                45.0f,
                (GLfloat)giWindowWidth / (GLfloat)giWindowHeight,
                0.1f,
                1000.0f);
            // for (int i = 0; i < numberOfLights * 2; i = i + 4)
            {
                pushMatrix(modelMatrix);
                {
                    translationMatrix = vmath::translate(0.2f, 0.0f, -2.5f);
                    modelMatrix = modelMatrix * translationMatrix;

                    glUniform1f(uniformExposure, Exposure);
                    glUniform1f(uniformDecay, Decay);
                    glUniform1f(uniformDensity, Density);
                    glUniform1f(uniformWeight, Weight);
                    glUniform1f(uniformIllumination, illuminator);

                    // for (int i = 0; i < numberOfLights * 2; i = i + 4)
                    // {
                    // glUniform2f(uniformLightLocation[0], lightAngles[i], lightAngles[i + 1]);
                    // glUniform2f(uniformLightLocation[1], lightAngles[i + 2], lightAngles[i + 3]);

                    // glUniform2f(uniformLightLocation[0], scaleX, scaleX);
                    // glUniform2f(uniformLightLocation[1], scaleY, scaleY);

                    // }
                    glUniform2f(uniformLightLocation[0], -0.07f + 0.250000f, 1.219f + 0.010000f);
                    glUniform2f(uniformLightLocation[1], 0.75f + -0.130000f, 1.219f);

                    // glUniform2f(uniformLightLocation[0], 0.0f, 3.0f);
                    // glUniform2f(uniformLightLocation[1], 0.0f, 3.0f);

                    glEnable(GL_TEXTURE_2D);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, fbo_texture_UserMap);
                    glUniform1i(textureSampler, 1);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, fbo_texture_ColorMap);
                    glUniform1i(textureSampler_ColorMapSampler, 0);

                    glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
                    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
                    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

                    glBindVertexArray(vao_quad);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    glBindVertexArray(0);

                    glDisable(GL_BLEND);
                }
                modelMatrix = popMatrix();
            }
        }
        glUseProgram(0);
        
    }

    void drawSphere(void)
    {
        glUseProgram(colorShader->shaderProgramObject);

        glUniformMatrix4fv(colorShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(colorShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(colorShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glBindVertexArray(vao_sphere);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere->getNumberOfSphereVertices());
        glBindVertexArray(0);

        glUseProgram(0);
    }

    vec2 getScreenLightPositionForGodRays(mat4 modelMatrix, mat4 viewMatrix, mat4 projectionMatrix)
    {
        //variable declarations
        // vec4 position = vec4(gfLightPosX, gfLightPosY, gfLightPosZ, 1.0f);
        vec4 position = vec4(objX, objY, objZ, 1.0f);
        vec2 lightPositionOnScreen;

        //code
        vec4 clipSpacePos = (position * viewMatrix) * projectionMatrix;

        //normalized Device Coordinate System
        vec3 ndcSpacePos = vec3(clipSpacePos[0] / clipSpacePos[3], clipSpacePos[1] / clipSpacePos[3], clipSpacePos[2] / clipSpacePos[3]);

        lightPositionOnScreen[0] = (ndcSpacePos[0] + 1.0f) / 2.0f;
        lightPositionOnScreen[1] = (ndcSpacePos[1] + 1.0f) / 2.0f;


        return (lightPositionOnScreen);
    }

    void texturedQuad(GLuint texture, GLfloat alpha)
    {
        // glUseProgram(commonShaders->textureShader->shaderProgramObject);

        // glUniformMatrix4fv(commonShaders->textureShader->modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        // glUniformMatrix4fv(commonShaders->textureShader->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        // glUniformMatrix4fv(commonShaders->textureShader->projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        // glUniform1f(commonShaders->textureShader->alphaValueUniform, alpha);

        // glEnable(GL_TEXTURE_2D);
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, texture);
        // glUniform1i(commonShaders->textureShader->textureSamplerUniform, 0);

        // glBindVertexArray(vao_quad);
        // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // glBindVertexArray(0);

        // glDisable(GL_BLEND);

        // glUseProgram(0);
    }

    void uninitialize()
    {
        delete (sphere);
        // delete (cone);

        // flag_web->uninitialize();
        // jokerModelShader.uninitialize_ModelShaderObject();
    }
};

#endif