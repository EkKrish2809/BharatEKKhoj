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
    GLuint viewMatrixUniform;       // model View Projection
    GLuint modelMatrixUniform;      // model View Projection

    GLuint heightMapSamplerUniform;
    GLuint terrainTextureUniform;
    GLuint terrainClipPlaneUniform;

    // For normal mapping
    GLuint lightPosUniform;
    GLuint viewPosUniform;
    GLuint normalMapSamplerUniform;

    GLuint terrainVAO, terrainVBO;

    GLuint texture_heightmap, texture_heightmap2;
    unsigned char *imageData = NULL;
    int numImageComponents = 0;
    GLuint texture_ground1, texture_ground2;
    int width, height;
    std::vector<float> vertices;

    float *posArray = NULL;
    float *texArray = NULL;

    // float lightPos[3] = {0.5f, 1.0f, 0.3f};

    Bool initialize(void)
    {
        GLint maxTessLevel;
        glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
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
        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);
        if (bShaderLinkStatus == False)
            return False;

        printf("Linking terrain shader\n");
        // post link - getting
        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

        terrainClipPlaneUniform = glGetUniformLocation(shaderProgramObject, "u_plane");
        heightMapSamplerUniform = glGetUniformLocation(shaderProgramObject, "heightMap");
        terrainTextureUniform = glGetUniformLocation(shaderProgramObject, "terrainTex");

        // lightPosUniform = glGetUniformLocation(shaderProgramObject, "lightPos");
        // viewPosUniform = glGetUniformLocation(shaderProgramObject, "viewPos");
        // normalMapSamplerUniform = glGetUniformLocation(shaderProgramObject, "normalMap");

        printf("Uniform locations\n");

        // loading texture for terrain
        if (LoadGlTexture(&texture_ground2, "./assets/textures/rock_06_diff_4k.jpg") == False)
        {
            printf("Failed to load terrain texture\n");
            return False;
        }
        printf("Terrain texture\n");

        // loading texture for terrain
        if (LoadGlTexture(&texture_ground1, "./assets/textures/terrain/lava.jpg") == False)
        {
            printf("Failed to load terrain texture\n");
            return False;
        }
        printf("Terrain texture\n");

        // Loading heightmap
        if (loadHeightMap(&texture_heightmap, "./assets/textures/iceland_heightmap.png") == False)
        {
            PrintLog("Failed to load heightMap\n");
            printf("Failed to load heightMap\n");
            return False;
        }
        if (loadHeightMap(&texture_heightmap2, "./assets/textures/newHieghtMap.bmp") == False)
        {
            PrintLog("Failed to load heightMap\n");
            printf("Failed to load heightMap\n");
            return False;
        }
        printf("terrain heightmap\n");
        printf("HeightMap::width = %f\theight = %f\n", (float)width, (float)height);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        // std::vector<float> vertices;

           unsigned int rez = 35;
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
        
        // Calculating normals
        // for (int i = 0; i < vertices.size(); i++)
        {
            // positions
            // vec3 pos1(vertices[i],  vertices[i+1], vertices[i+2]);
            // vec2 uv1(vertices[i+3], vertices[i+4]);
            // vec3 pos2(vertices[i+5],  vertices[i+6], vertices[i+7]);
            // vec2 uv2(vertices[i+8], vertices[i+9]);
            // vec3 pos3( vertices[i+10],  vertices[i+11], vertices[i+12]);
            // vec2 uv3(vertices[i+13], vertices[i+14]);  
            // vec3 pos4(vertices[i+15],  vertices[i+16], vertices[i+17]);
            // vec2 uv4(vertices[i+18], vertices[i+19]);
            // texture coordinates
            
            
            
            
            // normal vector
            // vec3 nm(0.0f, 0.0f, 1.0f);

            // // calculate tangent/bitangent vectors of both triangles
            // vec3 tangent1, bitangent1;
            // vec3 tangent2, bitangent2;
            // // triangle 1
            // // ----------
            // vec3 edge1 = pos1 - pos3;
            // vec3 edge2 = pos2 - pos3;
            // vec2 deltaUV1 = uv1 - uv3;
            // vec2 deltaUV2 = uv2 - uv3;

            // float f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

            // tangent1.x = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
            // tangent1.y = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
            // tangent1.z = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);

            // bitangent1.x = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
            // bitangent1.y = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
            // bitangent1.z = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);

            // triangle 2
            // ----------
            // edge1 = pos2 - pos3;
            // edge2 = pos4 - pos3;
            // deltaUV1 = uv2 - uv3;
            // deltaUV2 = uv4 - uv3;

            // f = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV2[0] * deltaUV1[1]);

            // tangent2.x = f * (deltaUV2[1] * edge1[0] - deltaUV1[1] * edge2[0]);
            // tangent2.y = f * (deltaUV2[1] * edge1[1] - deltaUV1[1] * edge2[1]);
            // tangent2.z = f * (deltaUV2[1] * edge1[2] - deltaUV1[1] * edge2[2]);


            // bitangent2.x = f * (-deltaUV2[0] * edge1[0] + deltaUV1[0] * edge2[0]);
            // bitangent2.y = f * (-deltaUV2[0] * edge1[1] + deltaUV1[0] * edge2[1]);
            // bitangent2.z = f * (-deltaUV2[0] * edge1[2] + deltaUV1[0] * edge2[2]);

        }

        // printing vertices
        // PrintLog("x = %f	y = %f	z = %f\n",vertices[0], vertices[1], vertices[3]);

        // vao , vbo related code
        glGenVertexArrays(1, &terrainVAO);
        glBindVertexArray(terrainVAO);

        glGenBuffers(1, &terrainVBO);
        glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sizeof(posArray), posArray, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        // texCoord attribute
        glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(sizeof(float) * 3));
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);

        glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);

        return True;
    }

    Bool loadHeightMap(GLuint *texture, const char *path)
    {
        // unsigned char *imageData = NULL;

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

    Bool loadHeightMapSTBI(GLuint *texture, const char *imageFilePath)
    {
        // variable declarations
        int image_width = 0;
        int image_height = 0;
        // int numImageComponents = 0;
        unsigned char *image_data = NULL;
        GLenum image_format = 0;

        // code
        glGenTextures(1, texture);
        glActiveTexture(GL_TEXTURE0);
        image_data = stbi_load(imageFilePath, &width, &height, &numImageComponents, 0);
        if (image_data != NULL)
        {
            if (numImageComponents == 1)
                image_format = GL_RED;
            else if (numImageComponents == 3)
                image_format = GL_RGB;
            else if (numImageComponents == 4)
                image_format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, *texture);

            // if (image_format == GL_RGBA)
            // {
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // }

            // else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            // glTexImage2D(GL_TEXTURE_2D, 0, image_format, width, image_height, 0, height, GL_UNSIGNED_BYTE, (void *)image_data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(image_data);
            return (True);
        }
        return (False);
    }

    void update(void)
    {
    }

    void display(int scene)
    {
        // code
        glUseProgram(shaderProgramObject);
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

        GLfloat eqn[] = {0.0f, 1.0f, 0.0f, 1.5f};

        if (scene == 1)
        {
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(heightMapSamplerUniform, 0);
            glBindTexture(GL_TEXTURE_2D, texture_heightmap);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_ground1);
            glUniform1i(terrainTextureUniform, 1);
        }
        else
        {
            glEnable(GL_CLIP_PLANE0);

            glActiveTexture(GL_TEXTURE0);
            glUniform1i(heightMapSamplerUniform, 0);
            glBindTexture(GL_TEXTURE_2D, texture_heightmap2);

            // glUniform4fv(terrainClipPlaneUniform, 1, eqn);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_ground2);
            glUniform1i(terrainTextureUniform, 1);
        }
        // render the terrain
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * 35 * 35);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }

    void uninitialize(void)
    {
        UninitializeShaders(shaderProgramObject);
    }
};
