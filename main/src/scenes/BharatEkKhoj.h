#pragma once

#include "../utils/GLHeadersAndMacros.h"
#include "../utils/GLShaders.h"
#include "../utils/common.h"
#include "../utils/Noise.h"

#include "../../includes/vmath.h"
#include "../../includes/sphere.h"

#include "../effects/Wood_Shader.h"
#include "../effects/rain/Rain.h"
#include "../effects/terrain/Terrain.h"
#include "../effects/fire/Fire.h"

using namespace vmath;

// Global class variables
WoodShader woodShader;
Sphere *sphere = new Sphere();
// SPHERE
GLuint vao_Sphere = 0;
GLuint vbo_Sphere_Position = 0;
GLuint vbo_Sphere_Texcoord = 0;
GLuint vbo_Sphere_Normal = 0;

// 3D Noise related variables
GLuint texture_noise = 0;
GLuint texture_noise_cloud = 0;
        
class BharatEkKhoj
{
    public:
        Rain *rain = NULL;
        Terrain *terrain = NULL;
        Fire *fire = NULL;

        BharatEkKhoj()
        {
            rain = new Rain();
            terrain = new Terrain();
            fire = new Fire();
        }

        Bool initialize(void)
        {
            // code
            // printf("test 1\n");
            // Bool status = woodShader.initialize();
            // if (!status)
            // {
            //     PrintLog("WoodShader::initialize() Failed\n");
            //     return False;
            // }
            // printf("test 2\n");


            // // initializing Sphere
            // sphere->generateSphereData1(1.0f, 50, 50);
            // printf("test 3\n");

            // // Sphere vao and vbo related code
            // // vao_Sphere
            // glGenVertexArrays(1, &vao_Sphere);
            // glBindVertexArray(vao_Sphere);

            // // vbo for position
            // glGenBuffers(1, &vbo_Sphere_Position);
            // glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Position);
            // glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereVertices()) * sizeof(float), sphere->getSphereVertex(), GL_STATIC_DRAW);
            // glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            // glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);

            // glBindBuffer(GL_ARRAY_BUFFER, 0);

            // // vbo for Normal
            // glGenBuffers(1, &vbo_Sphere_Normal);
            // glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normal);
            // glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereNormal()) * sizeof(float), sphere->getSphereNormal(), GL_STATIC_DRAW);
            // glVertexAttribPointer(MATRIX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            // glEnableVertexAttribArray(MATRIX_ATTRIBUTE_NORMAL);
            // glBindBuffer(GL_ARRAY_BUFFER, 0);

            // // vbo for texture
            // glGenBuffers(1, &vbo_Sphere_Normal);
            // glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normal);
            // glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereTexcoord()) * sizeof(float), sphere->getSphereTexcoord(), GL_STATIC_DRAW);
            // glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            // glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
            // glBindBuffer(GL_ARRAY_BUFFER, 0);


            // glBindVertexArray(0);

            // CreateNoise3D(&texture_noise);
            // CreateNoise3D(&texture_noise_cloud);
            // printf("Noise Texture IDs: %d, %d\n", texture_noise, texture_noise_cloud);

            if (!rain->initialize())
            {
                PrintLog("\nFailed to initialize rain\n");
                return False;
            }

            if (terrain->initialize() == False)
            {
                PrintLog("Failed to initialize terrain\n");
                return False;
            }

            if (fire->initializeFire() == False)
            {
                PrintLog("Failed to initialize fire\n");
                return False;
            }

            return True;
        }

        void display(void)
        {
            // code
            mat4 translationMatrix = mat4::identity();
            mat4 rotateMatrix = mat4::identity();
            mat4 scaleMatrix = mat4::identity();

            

            pushMatrix(modelMatrix);
            {
                terrain->display();
            }
            modelMatrix = popMatrix();

            pushMatrix(modelMatrix);
            {
                // fire->renderFire();
            }
            modelMatrix = popMatrix();

            pushMatrix(modelMatrix);
            {
                // // sinewave->display();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                rain->display();
                glDisable(GL_BLEND);
            }
            modelMatrix = popMatrix();
        }

        void update(void)
        {
            // code
            // fire->updateFire();
        }

        void uninitialize(void)
        {
            // code
            // woodShader.uninitialize();
            // rain->uninitialize();

            fire->uninitializeFire();

            if (fire)
            {
                delete fire;
                fire = NULL;
            }
        }

};