#pragma once

#include "../utils/GLHeadersAndMacros.h"
#include "../utils/GLShaders.h"
#include "../utils/common.h"
#include "../utils/Noise.h"
#include "../utils/BezierCamera.h"

#include "../../includes/vmath.h"
#include "../../includes/sphere.h"

#include "../effects/Wood_Shader.h"
#include "../effects/rain/Rain.h"
#include "../effects/starfield/StarField.h"
#include "../effects/terrain/Terrain.h"
#include "../effects/fire/Fire.h"
#include "../effects/grass/Grass.h"
#include "../effects/water/WaterMain.h"
#include "../effects/shadow/Shadow.h"
#include "../effects/metaball/MetaBall.h"
// #include "../effects/bloom/Bloom_Shaders.h"

#include "../shaders/sunsurface/SunSurface.h"

using namespace vmath;

void setGlobalBezierCamera(BezierCamera *bezierCamera);

// Global class variables
WoodShader woodShader;

class BharatEkKhoj
{
public:
    WaterMain *water = NULL;
    Rain *rain = NULL;
    Terrain *terrain = NULL;
    // BloomShaders *bloom = NULL;
    Fire *fire = NULL;
    Grass *grass = NULL;
    Starfield *starField = NULL;
    Shadow *shadow = NULL;
    MetaBall *metaball = NULL;
    SunSurfaceShader *sunSurface = NULL;
    CloudShader cloud;

    Sphere *sphere = NULL;

    /* CAMERA */
    BezierCamera *sceneOneCam = NULL;
    // SPHERE
    GLuint vao_Sphere = 0;
    GLuint vbo_Sphere_Position = 0;
    GLuint vbo_Sphere_Texcoord = 0;
    GLuint vbo_Sphere_Normal = 0;

    // 3D Noise related variables
    GLuint texture_noise = 0;
    GLuint texture_noise_cloud = 0;

    float sunTranslate = 0.0f;

    // std::vector<std::vector<float>> bezierPoints = {
    //     {114.700279f, 13.609997f, 119.850296f}};
    // std::vector<float> yaw = {
    //     -133.000000f};
    // std::vector<float> pitch = {
    //     0.000000f,
    // };

    // std::vector<std::vector<float>> camSceneOne = {
    //     // {22.000000f, 0.000000f, 16.000000f},
    //     // {-1.000000f, 0.000000f, 16.000000f},
    //     // {-1.000000f, 0.000000f, 4.000000f},
    //     {-1.000000f, 0.000000f, 4.000000f},
    // };

    // std::vector<float> yawSceneOne = {
    //     // -90.000000f, -90.000000f, -90.000000f,
    //     -90.000000f,
    // };

    // std::vector<float> pitchSceneOne = {
    //     // 0.000000f, 0.000000f, 0.000000f,
    //     0.000000f,
    // };

    // int vectorIndex = camSceneOne.size() - 1;
    // BezierCamera *cam1;
    bool showBezierCam = false;
    // BezierCamera *currentCamera;

    int CURRENT_SCENE = SCENE_1;

    BharatEkKhoj()
    {
        water = new WaterMain();
        rain = new Rain();
        terrain = new Terrain();
        // bloom = new BloomShaders();
        fire = new Fire();
        grass = new Grass();
        starField = new Starfield();
        shadow = new Shadow();
        metaball = new MetaBall();
        sunSurface = new SunSurfaceShader();

        sphere = new Sphere();

        /* CAMERA */
        sceneOneCam = new BezierCamera();
    }

    Bool initialize(void)
    {
        // code
        // vectorIndex = bezierPoints.size() - 1;

        if (water->initialize_WaterMain() == False)
        {
            PrintLog("Failed to initialize_WaterMain\n");
            printf("Failed to initialize_WaterMain\n");
            return False;
        }

        if (!rain->initialize())
        {
            PrintLog("\nFailed to initialize rain\n");
            printf("\nFailed to initialize rain\n");
            return False;
        }

        // if (fire->initializeFire() == False)
        // {
        //     PrintLog("Failed to initialize fire\n");
        //     return False;
        // }

        // if (grass->initialize() == False)
        // {
        //     PrintLog("Failed to initialize grass\n");
        //     return False;
        // }

        if (starField->initialize() == False)
        {
            PrintLog("Failed to initialize grass\n");
            return False;
        }

        // if (bloom->initialize() == False)
        // {
        //     PrintLog("Failed to initialize bloom\n");
        //     return False;
        // }

        if (terrain->initialize() == False)
        {
            PrintLog("Failed to initialize terrain\n");
            return False;
        }

        // if (shadow->initialize() == False)
        // {
        //     PrintLog("Failed to initialize shadow\n");
        //     printf("Failed to initialize shadow\n");
        //     // shadow->uninitialize();
        //     return False;
        // }

        // if (metaball->initialize() == False)
        // {
        //     PrintLog("Failed to initialize Metaball\n");
        //     printf("Failed to initialize Metaball\n");
        //     return False;
        // }

        sunSurface->initialize_SunSurafaceShaderObject();
        cloud.initialize();

        /* CAMERA */
        // sceneOneCamera();

        sphere->generateSphereData1(1.0f, 50, 50);
        // Sphere vao and vbo related code
        // vao_Sphere
        glGenVertexArrays(1, &vao_Sphere);
        glBindVertexArray(vao_Sphere);

        // vbo for position
        glGenBuffers(1, &vbo_Sphere_Position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
        glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereVertices()) * sizeof(float), sphere->getSphereVertex(), GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for Normal
        glGenBuffers(1, &vbo_Sphere_Normal);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normal);
        glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereNormal()) * sizeof(float), sphere->getSphereNormal(), GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_NORMAL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for texture
        glGenBuffers(1, &vbo_Sphere_Texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Texcoord);
        glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereTexcoord()) * sizeof(float), sphere->getSphereTexcoord(), GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        CreateNoise3D(&texture_noise);
        CreateNoise3D(&texture_noise_cloud);
        printf("Noise Texture IDs: %d, %d\n", texture_noise, texture_noise_cloud);

        // Temporary camera
        // cam1 = new BezierCamera();
        // cam1->initialize();
        // cam1->time = 0.1f;

        return True;
    }

    void sceneOneCamera(void)
    {
        // code
        // sceneOneCam->initialize();

        // std::vector<std::vector<float>> camSceneOne = {
        //     // {22.000000f, 0.000000f, 16.000000f},
        //     // {-1.000000f, 0.000000f, 16.000000f},
        //     // {-1.000000f, 0.000000f, 4.000000f},
        //     {-1.000000f, 0.000000f, 4.000000f},
        // };

        // std::vector<float> yawSceneOne = {
        //     // -90.000000f, -90.000000f, -90.000000f,
        //      -90.000000f,
        // };

        // std::vector<float> pitchSceneOne = {
        //     // 0.000000f, 0.000000f, 0.000000f,
        //     0.000000f,
        // };

        // setGlobalBezierCamera(sceneOneCam);

        // sceneOneCam->setBezierPoints(camSceneOne, yawSceneOne, pitchSceneOne);
        // sceneOneCam->update();
    }

    void display(double time)
    {
        // code
        mat4 translationMatrix = mat4::identity();
        mat4 rotateMatrix = mat4::identity();
        mat4 scaleMatrix = mat4::identity();

        // camSceneOne[vectorIndex][0] = objX;
        // camSceneOne[vectorIndex][1] = objY;
        // camSceneOne[vectorIndex][2] = objZ;
        // yawSceneOne[vectorIndex] = scaleX;
        // pitchSceneOne[vectorIndex] = scaleY;

        // printf("camSceneOne[vectorIndex][0] = %f\n", camSceneOne[vectorIndex][0]);
        // printf("yawSceneOne[vectorIndex] = %f\n", yawSceneOne[vectorIndex]);

        // setGlobalBezierCamera(sceneOneCam);
        // sceneOneCam->update();
        // viewMatrix = sceneOneCam->getViewMatrix();

        // cam1->setBezierPoints(bezierPoints, yaw, pitch);
        // cam1->update();
        // if (showBezierCam)
        // if ( currentCamera->time > 1.0f)
        // {
        //     float timeBckp = currentCamera->time;

        //     currentCamera->time = 1.0f;
        //     currentCamera->update();

        //     viewMatrix = currentCamera->getViewMatrix();
        //     currentCamera->time = timeBckp;
        //     currentCamera->update();
        // }
        // else
        // {
        //     viewMatrix = currentCamera->getViewMatrix();
        // }

        // if (time > SCENE_0_TIMER && time < SCENE_2_TIMER)
        if (CURRENT_SCENE == SCENE_1)
        {
            pushMatrix(perspectiveProjectionMatrix);
            pushMatrix(viewMatrix);
            {
                // perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 0.1f, 20000.0f);

                pushMatrix(modelMatrix);
                pushMatrix(viewMatrix);
                {
                    modelMatrix = mat4::identity();
                    // printf("%f is current time\n", time);
                    starField->renderStarField_Two(time);
                }
                viewMatrix = popMatrix();
                modelMatrix = popMatrix();

                
                pushMatrix(modelMatrix);
                {
                    pushMatrix(modelMatrix);
                    {
                        modelMatrix = mat4::identity();
                        sunSurfaceSphere(0.0f, modelMatrix);
                    }
                    modelMatrix = popMatrix();

                    if (time > SCENE_1_TIMER)
                    // if (sceneOneCam->time > 0.25f)
                    {
                        pushMatrix(modelMatrix);
                        {
                            modelMatrix = mat4::identity();
                            scaleMatrix = vmath::scale(0.4f, 0.4f, 0.4f);
                            // translationMatrix = vmath::translate(0.0f, 0.0f, 0.0f);
                            modelMatrix = translationMatrix * scaleMatrix;
                            sunSurfaceSphere(sunTranslate, modelMatrix);
                            // sunSurfaceSphere(50.0f, modelMatrix);
                        }
                        modelMatrix = popMatrix();

                        // update
                        if (sunTranslate < 50.0f)
                            sunTranslate += 0.008f;
                    }
                }
                modelMatrix = popMatrix();

                    
                
            }
            viewMatrix = popMatrix();
            perspectiveProjectionMatrix = popMatrix();

            printf("SCENE = %d\n", CURRENT_SCENE);
        }

        //    else if (time > SCENE_2_TIMER && time < SCENE_3_TIMER)
        else if (CURRENT_SCENE == SCENE_2)
        {
            // printf("%f is current time\n", time);
            pushMatrix(perspectiveProjectionMatrix);
            // pushMatrix(viewMatrix);
            {
                // perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)giWindowWidth / (GLfloat)giWindowHeight, 0.1f, 20000.0f);

                
                
                pushMatrix(modelMatrix);
                // pushMatrix(viewMatrix);
                {
                    // SCENE 2
                    // bloom->bindBloomFBO();
                    // {
                        modelMatrix = mat4::identity();
                        modelMatrix = vmath::scale(3.0f, 1.0f, 2.0f);
                        terrain->display(1);
                    // }
                    // bloom->unbindBloomFBO();

                    // bloom->renderBlurFBO();

                    // bloom->renderFinalBloomScene(11);
                    // bloom->exposure = 10.0f;
                }
                // viewMatrix = popMatrix();
                modelMatrix = popMatrix();

                pushMatrix(modelMatrix);
                pushMatrix(viewMatrix);
                {
                    // viewMatrix = mat4::identity();
                    // modelMatrix = mat4::identity();
                    // modelMatrix *= vmath::scale(10000.0f, 10000.0f, 1000.0f);
                    // cloud.renderClouds(texture_noise_cloud, vao_Sphere, sphere->getNumberOfSphereVertices(), time);
                }
                viewMatrix = popMatrix();
                modelMatrix = popMatrix();

                pushMatrix(modelMatrix);
                {
                    modelMatrix *= vmath::translate(0.0f, 10.0f, 0.0f);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    rain->display(time);
                    glDisable(GL_BLEND);
                }
                modelMatrix = popMatrix();

                // pushMatrix(modelMatrix);
                // {
                //     grass->display(time);
                // }
                // modelMatrix = popMatrix();
            }
            // viewMatrix = popMatrix();
            perspectiveProjectionMatrix = popMatrix();
            printf("SCENE = %d\n", CURRENT_SCENE);
        }
        //    else if (time > SCENE_3_TIMER && time < SCENE_4_TIMER)
        else if (CURRENT_SCENE == SCENE_3)
        {
            pushMatrix(modelMatrix);
            {
                

                pushMatrix(modelMatrix);
                // pushMatrix(viewMatrix);
                {
                    // modelMatrix = mat4::identity();
                    // printf("%f is current time\n", time);
                    glEnable(GL_BLEND);
                    water->display_WaterMain(texture_noise_cloud, vao_Sphere, sphere->getNumberOfSphereVertices(), time);
                    glDisable(GL_BLEND);
                }
                // viewMatrix = popMatrix();
                modelMatrix = popMatrix();

                // pushMatrix(modelMatrix);
                // pushMatrix(viewMatrix);
                // {
                //     // viewMatrix = mat4::identity();
                //     // modelMatrix = mat4::identity();
                //     // modelMatrix = vmath::scale(1000.0f, 1000.0f, 1000.0f);
                //     cloud.renderClouds(texture_noise_cloud, vao_Sphere, sphere->getNumberOfSphereVertices(), time);
                // }
                // viewMatrix = popMatrix();
                // modelMatrix = popMatrix();

                pushMatrix(modelMatrix);
                {
                    modelMatrix *= vmath::translate(0.0f, 10.0f, 0.0f);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    rain->display(time);
                    glDisable(GL_BLEND);
                }
                modelMatrix = popMatrix();
            }
            modelMatrix = popMatrix();
        

            printf("SCENE = %d\n", CURRENT_SCENE);
        }

        /*    else if (time > SCENE_4_TIMER && time < SCENE_5_TIMER)
            {
                pushMatrix(modelMatrix);
                {
                    fire->renderFire(time);
                }
                modelMatrix = popMatrix();
            }
            else if (time > SCENE_5_TIMER )
        */
        {
            // pushMatrix(modelMatrix);
            // {
            //     glEnable(GL_BLEND);
            //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //     rain->display();
            //     glDisable(GL_BLEND);
            // }
            // modelMatrix = popMatrix();
        } {
            /* Shadow */
            // pushMatrix(modelMatrix);
            // {
            //     shadow->display();
            // }
            // modelMatrix = popMatrix();
        }

        {
            /* Metaballs */
            // pushMatrix(modelMatrix);
            // {
            //     metaball->display(vao_Sphere, sphere->getNumberOfSphereVertices(), time);
            // }
            // modelMatrix = popMatrix();
        }
    }

    void sunSurfaceSphere(float translateX, mat4 matrix)
    {
        // code
        mat4 translationMatrix = mat4::identity();
        mat4 rotateMatrix = mat4::identity();
        mat4 scaleMatrix = mat4::identity();

        pushMatrix(modelMatrix);
        // pushMatrix(viewMatrix);
        {
            // modelMatrix = mat4::identity();
            glUseProgram(sunSurface->shaderProgramObject);

            translationMatrix = vmath::translate(translateX - 2.0f, 0.0f, 0.0f);
            rotateMatrix = vmath::rotate(90.0f, 1.0f, 0.0f, 0.0f);
            scaleMatrix = vmath::scale(2.0f, 2.0f, 2.0f);

            modelMatrix = modelMatrix * translationMatrix * scaleMatrix;

            // modelMatrix = translationMatrix * rotateMatrix;

            glUniformMatrix4fv(sunSurface->projectMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
            glUniformMatrix4fv(sunSurface->viewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(sunSurface->modelMatrixUniform, 1, GL_FALSE, modelMatrix);

            sunSurface->update_SunSurfaceObjectUniforms();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_3D, texture_noise_cloud);
            glUniform1i(sunSurface->textureSamplerUniform, 0);

            glBindVertexArray(vao_Sphere);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere->getNumberOfSphereVertices());

            glBindVertexArray(0);
            // unuse the shader program object
            glUseProgram(0);
        }
        // viewMatrix = popMatrix();
        modelMatrix = popMatrix();
    }

    void update(void)
    {
        // code
        // fire->updateFire();
        // / Temorary camera update
        // if (cam1->time < 1.0f)
        //     cam1->time += 0.0007f;
        printf("SCENE in update 2 = %d\n", CURRENT_SCENE);
        if (CURRENT_SCENE == SCENE_1)
            starField->updateTime();
        // grass->update();

        // if (sunTranslate < 100.0f)
        //     sunTranslate += 0.004f;

        // if (sceneOneCam->time < 1.0f)
        // {
        //     sceneOneCam->time += 0.0001f;
        //     // sceneOneCam->time = objX;
        //     // printf("CAM Time = %f\n", objX);
        // }
    }

    void setScene(void)
    {
        if (CURRENT_SCENE <= 2)
            CURRENT_SCENE = CURRENT_SCENE + 1;

        // CURRENT_SCENE = SCENE_3;
    }

    int getScene(void)
    {
        return CURRENT_SCENE;
    }

    void uninitialize(void)
    {
        // code
        // shadow->uninitialize();
        // if (shadow)
        // {
        //     delete shadow;
        //     shadow = NULL;
        // }
        // woodShader.uninitialize();
        rain->uninitialize();
        if (rain)
        {
            delete rain;
            rain = NULL;
        }

        // fire->uninitializeFire();
        // if (fire)
        // {
        //     delete fire;
        //     fire = NULL;
        // }

        terrain->uninitialize();
        if (terrain)
        {
            delete terrain;
            terrain = NULL;
        }

        // grass->uninitialize();
        // if (grass)
        // {
        //     delete grass;
        //     grass = NULL;
        // }
    }
};
