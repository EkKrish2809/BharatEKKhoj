#pragma once

#include <iostream>
#include <vector>

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
// #include "../utils/OGLTime.h"
#include "../../../includes/vmath.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "../../../includes/std_image.h"

class Grass
{

public:
    GLuint shaderProgramObject;

    GLuint vao;
    GLuint vbo_position;
    
    GLuint projectionMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint modelMatrixUniform;
    GLuint cameraPositionUniform;
    GLuint timeUniform;
    GLuint texture1Uniform;
    GLuint texture2Uniform;

    std::vector<vec3> grassPosition;

    GLuint texture_grass, texture_flowmap;

    vec3 cameraPosition = vec3(0.0f, 1.0f, 3.0f);
    vec3 cameraEye = vec3(0.0f, 0.0f, -1.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int numFrames = 0;

    float cameraSpeed = 0.0f;

    bool initialize(void)
    {
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/grass/Grass.vs", VERTEX);
        GLuint geometryShaderObject = CreateAndCompileShaderObjects("./src/shaders/grass/Grass.gs", GEOMETRY);
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/grass/Grass.fs", FRAGMENT);

        shaderProgramObject = glCreateProgram();

        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, geometryShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);

        glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");
        
        // Link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

        if (bShaderLinkStatus == False)
            return false;

        projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
        viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
        modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
        texture1Uniform = glGetUniformLocation(shaderProgramObject, "u_grassTexture");
        texture2Uniform = glGetUniformLocation(shaderProgramObject, "u_wind");
        cameraPositionUniform = glGetUniformLocation(shaderProgramObject, "u_cameraPosition");
        timeUniform = glGetUniformLocation(shaderProgramObject, "u_time");
        
        // Grass Position
        srand(time(NULL));

        for (float x = -25.0f; x < 25.0f; x = x + 0.1f) // 1000
        {       
            for (float z = -25.0f; z <25.0f; z = z + 0.1f) // 1002
            {
                int randomNumberX = rand() % 10 + 1;
                int randomNumberZ = rand() % 10 + 1;

                grassPosition.push_back(vec3(x + (float)randomNumberX / 25.0f, 0, z + (float)randomNumberZ / 25.0f));
            }
        }

        printf("\nVector Size = %lu\n", grassPosition.size());

        // VAO and VBO Related Code

        // VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // VBO For Position
        glGenBuffers(1, &vbo_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
        glBufferData(GL_ARRAY_BUFFER, grassPosition.size() * sizeof(vmath::vec3), grassPosition.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Unbinding of VAO
        glBindVertexArray(0);

        if (loadGrassTextures(&texture_grass, "./assets/textures/grass/grass.png") == false)
            return false;
        if (loadGrassTextures(&texture_flowmap, "./assets/textures/grass/flowmap.png") == false)
            return false;

        return true;
    }

    bool loadGrassTextures(GLuint* texture, char* file)
    {
        // Variable Declarations
        int width, height;
        int channels_in_file;
        unsigned char* image;
        bool bResult = false;

        // Code
        stbi_set_flip_vertically_on_load(true);
        image = stbi_load(file, &width, &height, &channels_in_file, 0);
        stbi_set_flip_vertically_on_load(false);

        // Error Checking
        if (image)
        {
            bResult = true;

            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glGenTextures(1, texture);
            glBindTexture(GL_TEXTURE_2D, *texture);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Create The Texture
            if (channels_in_file == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            else if (channels_in_file == 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

            // Unbind Texture
            glBindTexture(GL_TEXTURE_2D, 0);

            // Free Image
            stbi_image_free(image);
        }
        else
        {
            printf("\nPNG Image Loading Failed : %s\n", stbi_failure_reason());
        }

        return bResult;
    }

    void display(double time)
    {
        // FPS
        float currentFrame = time;
        deltaTime = currentFrame - lastFrame;
        numFrames++;

        // printf("ELAPSED TIME = %f\n", ELAPSED_TIME);

        std::string FPS = std::to_string((1.0f / deltaTime) * numFrames).substr(0, 4);
        // std::cout << "\nFPS = " << FPS << std::endl;
        lastFrame = currentFrame;
        numFrames = 0;

        //E1 : Use the shader program object
        glUseProgram(shaderProgramObject);

        //mat4 viewMatrix = lookat(cameraPosition, cameraPosition + cameraEye, cameraUp);

        glUniform1i(texture1Uniform, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_grass); 
        
        glUniform1i(texture2Uniform, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_flowmap);

        glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
        glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix); 
        glUniform3fv(cameraPositionUniform, 1, cameraPosition);
        glUniform1f(timeUniform, time);

        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, grassPosition.size());
        glBindVertexArray(0);

        // E3 : Unuse the shader program object
        glUseProgram(0);
    }

    void update(void)
    {
       // cameraSpeed = 0.5 * deltaTime;
    
        cameraPosition = cameraPosition + cameraSpeed * cameraEye;
    }

    void uninitialize(void)
    {
        if (texture_flowmap)
        {
            glDeleteTextures(1, &texture_flowmap);
            texture_flowmap = 0;
        }
        
        if (texture_grass)
        {
            glDeleteTextures(1, &texture_grass);
            texture_grass = 0;
        }

        if (vbo_position)
        {
            glDeleteBuffers(1, &vbo_position);
            vbo_position = 0;
        }
        
        if (vao)
        {
            glDeleteVertexArrays(1, &vao);
            vao = 0;
        }

        UninitializeShaders(shaderProgramObject);
    }
};

