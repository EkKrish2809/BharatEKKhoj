#pragma once

#include "../utils/GLHeadersAndMacros.h"
#include "../utils/GLShaders.h"
#include "../utils/common.h"
#include "../utils/Noise.h"

#include "../../includes/vmath.h"
#include "../../includes/sphere.h"

#include "../effects/Wood_Shader.h"
// #include "../effects/metaball/MetaballKishor.h"
// #include "../effects/metaball/Native.h"
// #include "../effects/metaballs_new/MetaBalls_New.h"
#include "../effects/metaball/MetaBall.h"


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
        // Metaball *metaball = NULL;

        BharatEkKhoj()
        {
            
        }

        Bool initialize(void)
        {
            // code
            // metaball = new Metaball();
            // if (!metaball->initialize())
            // {
            //     PrintLog("Failed to initialize Metaball\n");
            //     return False;
            // }
            
            setupGraphics(WIN_WIDTH, WIN_HEIGHT);
            // initializeMetaballs();

            return True;
        }

        void display(float time)
        {
            // code
            mat4 translationMatrix = mat4::identity();
            mat4 rotateMatrix = mat4::identity();
            mat4 scaleMatrix = mat4::identity();

            pushMatrix(modelMatrix);
            {
                renderFrame(time);
                // metaball->renderMetaball();
                // displayMetaball();
            }
            modelMatrix = popMatrix();

        }

        void update(void)
        {
            // code
            // metaball->updateMetabsll();
        }

        void uninitialize(void)
        {
            // code
            // woodShader.uninitialize();
            // rain->uninitialize();
            // cleanup();
            
        }

};