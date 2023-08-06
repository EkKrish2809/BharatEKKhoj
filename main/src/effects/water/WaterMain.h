#pragma once

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "../../utils/Camera.h"

// #include "../../includes/vmath.h"
#include "../terrain/Terrain.h"
#include "../clouds/CloudShader.h"

#include "WaterQuad_Shader.h"
#include "WaterBedQuad_Shader.h"

using namespace vmath;

#define REFLECTION_FBO_WIDTH 1920
#define REFLECTION_FBO_HEIGHT 1080

#define REFRACTION_FBO_WIDTH 1920
#define REFRACTION_FBO_HEIGHT 1080

// #define WATER_WAVE_SPEED 0.0003f
#define WATER_WAVE_SPEED 0.003f

#define VIGNETTE_OUTER_RADIUS 0.05f
#define VIGNETTE_INNER_RADIUS 0.01f

extern BezierCamera *bezierCamera;
extern bool USE_FPV_CAM;

class WaterMain
{

public:
    // FBO-related variables
    // Terrain *terrain = NULL;
    Terrain terrain;
    Camera camera;
    CloudShader cloud;

    GLuint fbo_reflection;
    GLuint rbo_reflection;
    GLuint fbo_texture_reflection;

    GLuint fbo_refraction;
    GLuint rbo_texture_refraction;
    GLuint fbo_texture_refraction;

    // For Vignette
    GLfloat gfVignetteOuterRadius = VIGNETTE_OUTER_RADIUS;
    GLfloat gfVignetteInnerRadius = VIGNETTE_INNER_RADIUS;

    // Water
    GLuint vao_waterQuad;
    GLuint vbo_waterQuad_position;

    GLuint vao_waterBedQuad;
    GLuint vbo_waterBedQuad_position;

    GLuint texture_waterBedFloor = 0;
    GLuint textureSamplerUniform_waterBedQuad;
    GLuint texture_waterDUDVMap = 0;
    GLuint texture_waterNormalMap = 0;

    GLfloat moveFactor = 0.0f;

    vec3 cameraPosition = vec3(0.0f, 1.0f, 2.0f);
    vec3 cameraCenter = vec3(0.0f, 0.0f, 0.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

    // Shader Objects
    WaterQuadShader waterQuadShader;
    WaterBedQuadShader waterBedQuadShader;

    float waterLevel = -10.0f;

    int initializeWaterQuad(void)
    {
        // VERTICES
        const GLfloat waterQuadPositions[] =
            {
                -100.0f, -100.0f,
                -100.0f, 100.0f,
                100.0f, -100.0f,

                100.0f, -100.0f,
                -100.0f, 100.0f,
                100.0f, 100.0f};

        // VAO-VBO Related Code
        glGenVertexArrays(1, &vao_waterQuad);
        glBindVertexArray(vao_waterQuad);

        // VBO For Position
        glGenBuffers(1, &vbo_waterQuad_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_waterQuad_position);

        glBufferData(GL_ARRAY_BUFFER, sizeof(waterQuadPositions), waterQuadPositions, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        printf("water quad initialization\n");

        return (0);
    }

    int initializeWaterBedQuad(void)
    {
        // VERTICES
        const GLfloat waterBedQuadPositions[] =
            {
                -20.0f, -20.0f,
                -20.0f, 20.0f,
                20.0f, -20.0f,

                20.0f, -20.0f,
                -20.0f, 20.0f,
                20.0f, 20.0f};

        // VAO-VBO Related Code
        glGenVertexArrays(1, &vao_waterBedQuad);
        glBindVertexArray(vao_waterBedQuad);

        // VBO For Position
        glGenBuffers(1, &vbo_waterBedQuad_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_waterBedQuad_position);

        glBufferData(GL_ARRAY_BUFFER, sizeof(waterBedQuadPositions), waterBedQuadPositions, GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        printf("water bed quad initialization\n");

        return (0);
    }

    bool initializeReflectionFBO(GLint reflectionTextureWidth, GLint reflectionTextureHeight)
    {
        // variables
        int maxRenderbufferSize;

        // code
        // 1: Check Available renderbuffer size
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
        if (maxRenderbufferSize < reflectionTextureWidth || maxRenderbufferSize < reflectionTextureHeight)
        {
            PrintLog("Reflection : Insufficient Renderbuffer size\n");
            return (false);
        }

        // 2: Create Framebuffer object
        glGenFramebuffers(1, &fbo_reflection);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflection);

        // 3: Create Renderbuffer object
        glGenRenderbuffers(1, &rbo_reflection);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_reflection);

        // 4: Where to keep this Renderbuffer and what will be the format of Renderbuffer
        // i.e. storage and format of Renderbuffer
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, reflectionTextureWidth, reflectionTextureHeight); // Target, format, width, height

        // 5: Create Empty texture to be filled with scene later
        glGenTextures(1, &fbo_texture_reflection);
        glBindTexture(GL_TEXTURE_2D, fbo_texture_reflection);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, reflectionTextureWidth, reflectionTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        // 6: Give above texture to fbo
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture_reflection, 0);

        // 7: Give renderbuffer to fbo
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_reflection);

        // Check whether the framebuffer is created successfully or not
        GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (result != GL_FRAMEBUFFER_COMPLETE)
        {
            PrintLog("Reflection Framebuffer is not complete\n");
            return (false);
        }

        // unbind Framebuffer, when it is done, renderbuffer also gets automatically unbound.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return (true);
    }

    bool initializeRefractionFBO(GLint refractionTextureWidth, GLint refractionTextureHeight)
    {
        // variables
        int maxRenderbufferSize;

        // code
        // 1: Check Available renderbuffer size
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
        if (maxRenderbufferSize < refractionTextureWidth || maxRenderbufferSize < refractionTextureHeight)
        {
            PrintLog("Refraction : Insufficient Renderbuffer size\n");
            return (false);
        }

        // 2: Create Framebuffer object
        glGenFramebuffers(1, &fbo_refraction);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_refraction);

        // 3: Create Empty texture to be filled with scene later
        glGenTextures(1, &fbo_texture_refraction);
        glBindTexture(GL_TEXTURE_2D, fbo_texture_refraction);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, refractionTextureWidth, refractionTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        // 4: Give above texture to fbo
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture_refraction, 0);

        // 5: Create Empty texture to be filled with scene later
        glGenTextures(1, &rbo_texture_refraction);
        glBindTexture(GL_TEXTURE_2D, rbo_texture_refraction);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, refractionTextureWidth, refractionTextureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        // depth data is in float

        // 6: Give above texture to fbo
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rbo_texture_refraction, 0);

        // Check whether the framebuffer is created successfully or not
        GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (result != GL_FRAMEBUFFER_COMPLETE)
        {
            PrintLog("Refraction Framebuffer is not complete\n");
            return (false);
        }

        // unbind Framebuffer, when it is done, renderbuffer also gets automatically unbound.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return (true);
    }

    Bool initialize_WaterMain(void)
    {
        printf("starting WaterMain initialization\n");

        cloud.initialize();
        waterQuadShader.initialize();
        waterBedQuadShader.initialize();
        int iRetVal = -1;

        iRetVal = initializeWaterQuad();
        if (iRetVal != 0)
        {
            PrintLog("initializeWaterQuad() Failed.\n ");
            printf("initializeWaterQuad() Failed.\n ");
            // return (iRetVal);
            return False;
        }

        iRetVal = initializeWaterBedQuad();
        if (iRetVal != 0)
        {
            PrintLog("initializeWaterBedQuad() Failed.\n ");
            printf("initializeWaterBedQuad() Failed.\n ");
            // return (iRetVal);
            return False;
        }

        //-------------------------------------
        if (initializeReflectionFBO(REFLECTION_FBO_WIDTH, REFLECTION_FBO_HEIGHT) == false)
        {
            PrintLog("ReflectionFBO\n");
            printf("ReflectionFBO\n");
            // return (-6);
            return False;
        }

        if (initializeRefractionFBO(REFRACTION_FBO_WIDTH, REFRACTION_FBO_HEIGHT) == false)
        {
            PrintLog("RefractionFBO\n");
            printf("RefractionFBO\n");
            // return (-7);
            return False;
        }

        //-------------------------------------
        if (LoadPNGImage(&texture_waterDUDVMap, "./assets/textures/water/waterDUDV.png") == false)
        {
            PrintLog("LoadPNGImage Failed for waterDUDV\n");
            printf("LoadPNGImage Failed for waterDUDV\n");
            // return (-8);
            return False;
        }

        if (LoadPNGImage(&texture_waterNormalMap, "./assets/textures/water/waterNormalMap.png") == false)
        {
            PrintLog("LoadPNGImage Failed for waterNormalMap\n");
            printf("LoadPNGImage Failed for waterNormalMap\n");
            // return (-9);
            return False;
        }
        if (LoadPNGImage(&texture_waterBedFloor, "./assets/textures/water/sand.png") == false)
        {
            PrintLog("LoadPNGImage Failed for Sand\n");
            printf("LoadPNGImage Failed for Sand\n");
            // return (-10);
            return False;
        }

        if (terrain.initialize() == False)
        {
            PrintLog("Failed to initialize terrain\n");
            printf("Failed to initialize terrain\n");
            return False;
        }

        return True;
    }

    void update_WaterMain(void)
    {
    }

    void display_WaterMain(GLuint texture_noise_cloud, GLuint VAO, int sphere_vertices, float current_time)
    {
        // code
        pushMatrix(modelMatrix);
        {
            this->renderReflection(giWindowWidth, giWindowHeight, texture_noise_cloud, VAO, sphere_vertices, current_time);
            // this->renderReflection(REFLECTION_FBO_WIDTH, REFLECTION_FBO_HEIGHT);
            this->renderRefraction(giWindowWidth, giWindowHeight);
            // this->renderRefraction(REFRACTION_FBO_WIDTH, REFRACTION_FBO_HEIGHT);
        }
        modelMatrix = popMatrix();
        // As above functions have their own glClear() calls in their
        // definitions, these functions are called here, before this function's
        // glClear() call.
        pushMatrix(modelMatrix);
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glViewport(0, 0, (GLsizei)giWindowWidth, (GLsizei)giWindowHeight);
            perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 0.1f, 20000.0f);
            // printf("1. width : %d  Height : %d\n", giWindowWidth, giWindowHeight);
            pushMatrix(modelMatrix);
            {
                pushMatrix(modelMatrix);
                {
                    modelMatrix = mat4::identity();
                    // printf("%f is current time\n", time);
                    // modelMatrix = vmath::translate(150.000000f + objX, -100.000000f + objY, -224.000000f + objZ) * vmath::rotate(-146.000000f + scaleX, 1.0f, 0.0f, 0.0f)
                    //                 * vmath::rotate(181.000000f + scaleY, 0.0f, 1.0f, 0.0f) * vmath::rotate(scaleZ, 0.0f, 0.0f, 1.0f);
                     modelMatrix = vmath::translate(150.000000f , -100.000000f + waterLevel, -224.000000f + -8.0f) * vmath::rotate(-146.000000f + -240.0f, 1.0f, 0.0f, 0.0f)
                                    * vmath::rotate(181.000000f , 0.0f, 1.0f, 0.0f) * vmath::rotate(0.0f, 0.0f, 0.0f, 1.0f);

                    this->renderWaterQuad();
                }
                modelMatrix = popMatrix();
                // this->renderAboveWaterScene(false, bezierCamera->getCameraPosition(), cameraCenter, cameraUp, texture_noise_cloud, VAO, sphere_vertices, current_time);
                this->renderAboveWaterScene(false, camera.getEye(), camera.getCenter(), camera.getUp(), texture_noise_cloud, VAO, sphere_vertices, current_time);
                this->renderBelowWaterScene();

                
            }
            modelMatrix = popMatrix();
        }
        modelMatrix = popMatrix();

        if (waterLevel < 0.0f)
            waterLevel += 0.001f;
        // waterLevel = -10.0f;
    }

    void renderReflection(GLint textureWidth, GLint textureHeight, GLuint texture_noise_cloud, GLuint VAO, int sphere_vertices, float current_time)
    {
        
        // code
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflection);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, textureWidth, textureHeight);
        perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)textureWidth / (GLfloat)textureHeight, 0.1f, 20000.0f);
        // printf("2. width : %d  Height : %d\n", giWindowWidth, giWindowHeight);
        // Render scene to be Reflected
        // this->renderAboveWaterScene(true, cameraPosition, cameraCenter, cameraUp, texture_noise_cloud, VAO, sphere_vertices, current_time);
        this->renderAboveWaterScene(true, camera.getEye(), camera.getCenter(), camera.getUp(), texture_noise_cloud, VAO, sphere_vertices, current_time);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // for reflection
    // void bindReflectionFBO(GLint textureWidth, GLint textureHeight)
    // {
    //     // code
    //     glBindFramebuffer(GL_FRAMEBUFFER, fbo_reflection);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     glViewport(0, 0, textureWidth, textureHeight);
    //     perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)textureWidth / (GLfloat)textureHeight, 0.1f, 20000.0f);
    // }

    // void unBindReflectionFBO(void)
    // {
    //     // code
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void renderRefraction(GLint textureWidth, GLint textureHeight)
    {
        // Function Prototype
        // void renderBelowWaterScene(void);

        // code
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_refraction);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, textureWidth, textureHeight);
        perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)textureWidth / (GLfloat)textureHeight, 0.1f, 20000.0f);
        // printf("3. width : %d  Height : %d\n", giWindowWidth, giWindowHeight);
        // Render scene to be Refracted
        this->renderBelowWaterScene();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void renderWaterQuad(void)
    {
        // variable declarations
        mat4 translationMatrix;
        mat4 scaleMatrix;

        float lightColor[] = {1.0f, 1.0f, 1.0f}; // white light
        float lightPosition[] = {0.0f, 100.0f, 10.0f};

        // code
        pushMatrix(modelMatrix);
        pushMatrix(viewMatrix);
        {
            glUseProgram(waterQuadShader.shaderProgramObject_waterQuad);

            // TRANSFORMATIONS
            translationMatrix = mat4::identity();
            scaleMatrix = mat4::identity();

            scaleMatrix = vmath::scale(100.0f, 1.0f, 100.75f);
            modelMatrix = modelMatrix * scaleMatrix;
            viewMatrix = viewMatrix * vmath::lookat(cameraPosition, cameraCenter, cameraUp);

            // viewMatrix = camera.getViewMatrix();

            glUniformMatrix4fv(waterQuadShader.modelMatrixUniform_waterQuad, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(waterQuadShader.viewMatrixUniform_waterQuad, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(waterQuadShader.projectionMatrixUniform_waterQuad, 1, GL_FALSE, perspectiveProjectionMatrix);

            glUniform3fv(waterQuadShader.cameraPositionUniform_waterQuad, 1, cameraPosition);

            glUniform3fv(waterQuadShader.lightPositionUniform_waterQuad, 1, lightPosition);
            glUniform3fv(waterQuadShader.lightColorUniform_waterQuad, 1, lightColor);

            moveFactor = moveFactor + WATER_WAVE_SPEED;
            glUniform1f(waterQuadShader.moveFactorOffsetUniform_waterQuad, moveFactor);

            // // For Vignette
            // glUniform2f(waterQuadShader.resolutionUniform, (GLfloat)giWindowWidth, (GLfloat)giWindowHeight);

            // if (gfVignetteOuterRadius < 1.0f)
            // {
            //     gfVignetteOuterRadius = gfVignetteOuterRadius + 0.001f;
            //     glUniform1f(waterQuadShader.vignetteOuterRadiusUniform, gfVignetteOuterRadius);
            // }

            // if (gfVignetteInnerRadius < 0.8f)
            // {
            //     gfVignetteInnerRadius = gfVignetteInnerRadius + 0.001f;
            //     glUniform1f(waterQuadShader.vignetteInnerRadiusUniform, gfVignetteInnerRadius);
            // }

            // Following is multitexturing
            // Bind reflection texture
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(waterQuadShader.reflectionTextureSamplerUniform_waterQuad, 0);
            glBindTexture(GL_TEXTURE_2D, fbo_texture_reflection);

            // Bind refraction texture
            glActiveTexture(GL_TEXTURE1);
            glUniform1i(waterQuadShader.refractionTextureSamplerUniform_waterQuad, 1);
            glBindTexture(GL_TEXTURE_2D, fbo_texture_refraction);

            // Bind Water DUDVMap texture
            glActiveTexture(GL_TEXTURE2);
            glUniform1i(waterQuadShader.waterDUDVMapTextureSamplerUniform_waterQuad, 2);
            glBindTexture(GL_TEXTURE_2D, texture_waterDUDVMap);

            // Bind Water DUDVMap texture
            glActiveTexture(GL_TEXTURE3);
            glUniform1i(waterQuadShader.waterNormalMapTextureSamplerUniform_waterQuad, 3);
            glBindTexture(GL_TEXTURE_2D, texture_waterNormalMap);

            glBindVertexArray(vao_waterQuad);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, 0);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glUseProgram(0);
        }
        viewMatrix = popMatrix();
        modelMatrix = popMatrix();
    }

    void renderAboveWaterScene(bool bDisplayReflection, vec3 camPos, vec3 camCenter, vec3 camUp, GLuint texture_noise_cloud, GLuint VAO, int sphere_vertices, float current_time)
    {
        // function prototypes

        // code
        vec3 changedCameraPosition = vec3(0.0f, 0.0f, 0.0f);
        changedCameraPosition = camPos;

        if (bDisplayReflection == true)
        {
            changedCameraPosition[1] = changedCameraPosition[1] * -1.0f;
            // Invert Y of camera if reflection is displayed.
        }

        // if (USE_FPV_CAM)
		// {
		// 	camera.invertPitch();
		// 	viewMatrix = camera.getViewMatrix();
		// }
		// else
		// {
		// 	bezierCamera->invertPitch();
		// 	viewMatrix = bezierCamera->getViewMatrix();
		// }

        pushMatrix(modelMatrix);
        // pushMatrix(viewMatrix);
        {
            // viewMatrix = vmath::lookat(changedCameraPosition, camCenter, camUp);
            pushMatrix(modelMatrix);
            {
                modelMatrix = mat4::identity();
                modelMatrix = vmath::scale(10.0f, 15.0f, 10.0f);
                terrain.display(2);
            }
            modelMatrix = popMatrix();

            pushMatrix(modelMatrix);
            // pushMatrix(viewMatrix);
            {
                // viewMatrix = mat4::identity();
                // modelMatrix = mat4::identity();
                // modelMatrix *= vmath::scale(1000.0f, 1000.0f, 1000.0f);
                cloud.renderClouds(texture_noise_cloud, VAO, sphere_vertices, current_time);
            }
            // viewMatrix = popMatrix();
            modelMatrix = popMatrix();
        }
        // viewMatrix = popMatrix();
        modelMatrix = popMatrix();
    }

    void renderBelowWaterScene(void)
    {
        // function prototypes
        // void renderWaterBedQuad(void);

        // code
        // this->renderWaterBedQuad();
        pushMatrix(modelMatrix);
        {
            modelMatrix = mat4::identity();
            modelMatrix = vmath::scale(10.0f, 15.0f, 10.0f);
            terrain.display(2);
        }
        modelMatrix = popMatrix();
    }

    void renderWaterBedQuad(void)
    {
        // variable declarations
        mat4 translationMatrix;
        mat4 scaleMatrix;

        // code
        pushMatrix(modelMatrix);
        pushMatrix(viewMatrix);
        {
            glUseProgram(waterBedQuadShader.shaderProgramObject_waterBedQuad);

            // TRANSFORMATIONS
            translationMatrix = mat4::identity();
            scaleMatrix = mat4::identity();

            translationMatrix = vmath::translate(0.0f, -10.5f , 0.0f);
            // printf("objY : %f\n", objY);
            scaleMatrix = vmath::scale(100.0f, 1.0f, 100.75f);
            modelMatrix = modelMatrix  * translationMatrix * scaleMatrix;

            viewMatrix = viewMatrix * vmath::lookat(cameraPosition, cameraCenter, cameraUp);

            //    viewMatrix = camera.getViewMatrix();

            glUniformMatrix4fv(waterBedQuadShader.modelMatrixUniform_waterBedQuad, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(waterBedQuadShader.viewMatrixUniform_waterBedQuad, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(waterBedQuadShader.projectionMatrixUniform_waterBedQuad, 1, GL_FALSE, perspectiveProjectionMatrix);

            // Bind reflection texture
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(waterBedQuadShader.textureSamplerUniform_waterBedQuad, 0);
            glBindTexture(GL_TEXTURE_2D, texture_waterBedFloor);

            glBindVertexArray(vao_waterBedQuad);

            //	glVertexAttrib3f(RAMA_ATTRIBUTE_COLOR, 0.0f, 1.0f, 1.0f);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);

            glUseProgram(0);
        }
        viewMatrix = popMatrix();
        modelMatrix = popMatrix();

        // pushMatrix(modelMatrix);
        // {
        //     terrain.display(2);
        // }
        // modelMatrix = popMatrix();
    }

    void uninitialize_WaterMain(void)
    {
        // function prototypes
        // void uninitializeReflectionFBO(void);
        // void uninitializeRefractionFBO(void);

        // code
        if (texture_waterBedFloor)
        {
            glDeleteTextures(1, &texture_waterBedFloor);
            texture_waterBedFloor = 0;
        }

        if (texture_waterNormalMap)
        {
            glDeleteTextures(1, &texture_waterNormalMap);
            texture_waterNormalMap = 0;
        }

        if (texture_waterDUDVMap)
        {
            glDeleteTextures(1, &texture_waterDUDVMap);
            texture_waterDUDVMap = 0;
        }

        this->uninitializeRefractionFBO();
        this->uninitializeReflectionFBO();

        // ****** SHADER-RELATED UNINITIALIZATIONS ******
        UninitializeShaders(waterQuadShader.shaderProgramObject_waterQuad);
        UninitializeShaders(waterBedQuadShader.shaderProgramObject_waterBedQuad);

        if (vbo_waterBedQuad_position)
        {
            glDeleteBuffers(1, &vbo_waterBedQuad_position);
            vbo_waterBedQuad_position = 0;
        }

        if (vao_waterBedQuad)
        {
            glDeleteVertexArrays(1, &vao_waterBedQuad);
            vao_waterBedQuad = 0;
        }

        if (vbo_waterQuad_position)
        {
            glDeleteBuffers(1, &vbo_waterQuad_position);
            vbo_waterQuad_position = 0;
        }

        if (vao_waterQuad)
        {
            glDeleteVertexArrays(1, &vao_waterQuad);
            vao_waterQuad = 0;
        }
    }

    void uninitializeReflectionFBO(void)
    {
        //
        if (fbo_texture_reflection)
        {
            glDeleteTextures(1, &fbo_texture_reflection);
            fbo_texture_reflection = 0;
        }

        if (rbo_reflection)
        {
            glDeleteRenderbuffers(1, &rbo_reflection);
            rbo_reflection = 0;
        }

        if (fbo_reflection)
        {
            glDeleteFramebuffers(1, &fbo_reflection);
            fbo_reflection = 0;
        }
    }

    void uninitializeRefractionFBO(void)
    {
        if (rbo_texture_refraction)
        {
            glDeleteTextures(1, &rbo_texture_refraction);
            rbo_texture_refraction = 0;
        }

        if (fbo_texture_refraction)
        {
            glDeleteTextures(1, &fbo_texture_refraction);
            fbo_texture_refraction = 0;
        }

        if (fbo_refraction)
        {
            glDeleteFramebuffers(1, &fbo_refraction);
            fbo_refraction = 0;
        }
    }
};
