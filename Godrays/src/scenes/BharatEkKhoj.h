#pragma once

#include "../utils/GLHeadersAndMacros.h"
#include "../utils/GLShaders.h"
#include "../utils/common.h"
#include "../utils/Noise.h"

#include "../../includes/vmath.h"
#include "../../includes/sphere.h"

#include "../effects/Wood_Shader.h"
#include "../effects/godrays/Godrays.h"


using namespace vmath;

// Global class variables
WoodShader woodShader;
Sphere *sphere = NULL;
Godrays *godrays = NULL;
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
        

        BharatEkKhoj()
        {
            // godrays = new Godrays();
        }

        Bool initialize(void)
        {
            // code
      /*      // printf("test 1\n");
            Bool status = woodShader.initialize();
            if (!status)
            {
                PrintLog("WoodShader::initialize() Failed\n");
                return False;
            }
            // printf("test 2\n");


            // initializing Sphere
            sphere = new Sphere(1.0f, 50, 50);
            // sphere->generateSphereData1(1.0f, 50, 50);
            printf("test 3\n");

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
            glGenBuffers(1, &vbo_Sphere_Normal);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_Sphere_Normal);
            glBufferData(GL_ARRAY_BUFFER, (sphere->getNumberOfSphereTexcoord()) * sizeof(float), sphere->getSphereTexcoord(), GL_STATIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);


            glBindVertexArray(0);

            CreateNoise3D(&texture_noise);
            CreateNoise3D(&texture_noise_cloud);
            printf("Noise Texture IDs: %d, %d\n", texture_noise, texture_noise_cloud);
*/

            godrays = new Godrays();
            if (!godrays->initialize())
            {
                printf("Godrays :: Failed to load\n");
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
                godrays->display(2.0f);
                printf("scaleX : %f\n", scaleX);
                // modelMatrix = modelMatrix * vmath::translate(0.0f, 0.0f, -4.0f);
                // godrays->drawSphere();

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
            godrays->uninitialize();
            if (godrays)
            {
                free(godrays);
                godrays = NULL;
            }
            
        }

};