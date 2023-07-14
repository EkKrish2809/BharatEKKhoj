#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include <iostream>
#include <vector>

using namespace std;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int NUM_PATCH_PTS = 4;

class Terrain
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObject;

    GLuint projectionMatrixUniform; // model View Projection
    GLuint viewMatrixUniform;    // model View Projection
    GLuint modelMatrixUniform;   // model View Projection

    GLuint heightMapSamplerUniform;

    GLuint terrainVAO, terrainVBO;

    GLuint texture_heightmap;
    GLuint texture_ground;
    int width, height;
    
    Bool initialize(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/terrain/terrain.vs", VERTEX);

        // tessellation control shader
        GLuint tessCntShaderObject = CreateAndCompileShaderObjects("./src/shaders/terrain/terrain.tcs", TESSELLATION_CONTROL);

        // tessellation evaluation shader
        GLuint tessEvlShaderObject = CreateAndCompileShaderObjects("./src/shaders/terrain/terrain.tes", TESSELLATION_EVALUATION);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/terrain/terrain.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, tessCntShaderObject);
        glAttachShader(shaderProgramObject, tessEvlShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // Binding Color Array
        // glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_COLOR, "a_color");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);
        if (bShaderLinkStatus == False)
            return False;
            
        printf("Linking terrain shader\n");
        // post link - getting
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        heightMapSamplerUniform = glGetUniformLocation(shaderProgramObject, "heightMap");

        printf("Uniform locations\n");

        // loading texture for terrain
        if (LoadGlTexture(&texture_ground, "./assets/textures/fire/fire.png") == False)
        {
            printf("Failed to load terrain texture\n");
            return False;
        }
        printf("Terrain texture\n");

        // Loading heightmap
        if (loadHeightMap(&texture_heightmap, "./assets/textures/Height_Map_.png") == False)
        {
            PrintLog("Failed to load heightMap\n");
            printf("Failed to load heightMap\n");
            return False;
        }
        printf("terrain heightmap\n");
	printf("HeightMap::width = %f\theight = %f\n",(float)width, (float)height);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        std::vector<float> vertices;

        unsigned int rez = 20;
        for(unsigned int i = 0; i <= rez-1; i++)
        {
            for(unsigned int j = 0; j <= rez-1; j++)
            {
                vertices.push_back(-(float)width/2.0f + (float)width*i/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-(float)height/2.0f + (float)height*j/(float)rez); // v.z
                vertices.push_back(i / (float)rez); // u
                vertices.push_back(j / (float)rez); // v

                vertices.push_back(-(float)width/2.0f + (float)width*(i+1)/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-(float)height/2.0f + (float)height*j/(float)rez); // v.z
                vertices.push_back((i+1) / (float)rez); // u
                vertices.push_back(j / (float)rez); // v

                vertices.push_back(-(float)width/2.0f + (float)width*i/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-(float)height/2.0f + (float)height*(j+1)/(float)rez); // v.z
                vertices.push_back(i / (float)rez); // u
                vertices.push_back((j+1) / (float)rez); // v

                vertices.push_back(-(float)width/2.0f + (float)width*(i+1)/(float)rez); // v.x
                vertices.push_back(0.0f); // v.y
                vertices.push_back(-(float)height/2.0f + (float)height*(j+1)/(float)rez); // v.z
                vertices.push_back((i+1) / (float)rez); // u
                vertices.push_back((j+1) / (float)rez); // v
            }
        }
        std::cout << "Loaded " << rez*rez << " patches of 4 control points each" << std::endl;
        std::cout << "Processing " << rez*rez*4 << " vertices in vertex shader" << std::endl;

	// printing vertices
	PrintLog("x = %f	y = %f	z = %f\n",vertices[0], vertices[1], vertices[3]);

        // vao , vbo related code
         glGenVertexArrays(1, &terrainVAO);
        glBindVertexArray(terrainVAO);

        glGenBuffers(1, &terrainVBO);
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        // texCoord attribute
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);

        glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

        return True;
    }

    Bool loadHeightMap(GLuint *texture, const char *path)
    {
        unsigned char *imageData = NULL;

        // code
        imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
        if (imageData == NULL)
        {
		printf("Failed to load HeightMap\n");
            return False;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // gluBuild2DMipmaps
        glTexImage2D(GL_TEXTURE_2D, 
                0, 
                GL_RGB, 
                width, 
                height, 
                0,
                GL_RGB, 
                GL_UNSIGNED_BYTE, 
                imageData);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        SOIL_free_image_data(imageData);
        return True;
    }

    void update(void)
    {
    }

    void display(void)
    {
        // code
        glUseProgram(shaderProgramObject);
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_heightmap);
        glUniform1i(heightMapSamplerUniform, 0);

        // render the terrain
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*20*20);
        glBindVertexArray(0);

        glUseProgram(0);
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
