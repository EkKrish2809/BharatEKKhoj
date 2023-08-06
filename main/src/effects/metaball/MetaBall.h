#ifndef METABALL_H
#define METABALL_H

#include "../../utils/common.h"
#include "../../utils/GLShaders.h"

class MetaBall
{
    public:
        // Variables
        GLuint shaderProgramObject;
        GLuint projectionMatrixUniform;
        GLuint viewMatrixUniform;
        GLuint modelMatrixUniform;

        GLuint resolutionUniform;
        GLuint timeUniform;

        GLuint vao;
        GLuint vbo_position;
        GLuint vbo_texcoord;

        Bool initialize(void)
        {
            // vertex shader
            GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/metaball.vs", VERTEX);
            // fragment Shader
            GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/metaball.fs", FRAGMENT);

            shaderProgramObject = glCreateProgram();
            glAttachShader(shaderProgramObject, vertexShaderObject);
            glAttachShader(shaderProgramObject, fragmentShaderObject);

            // prelinked binding
            // Binding Position Array
            glBindAttribLocation(shaderProgramObject, MATRIX_ATTRIBUTE_POSITION, "a_position");

            // link
            Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject);

            if (bShaderLinkStatus == False)
                return False;

            // post link - getting
            projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
            viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
            modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");

            resolutionUniform = glGetUniformLocation(shaderProgramObject, "iResolution");
            timeUniform = glGetUniformLocation(shaderProgramObject, "iTime");

            // Declaration of Vertex Data Arrays
            const GLfloat position[] =
            {
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f
            };

            const GLfloat texcoord[] =
            {
                1.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f
            };
            

            // vao_fire and VBO Related Code
            // --------------------------------------------------------------------------------------------------
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // VBO For Position
            glGenBuffers(1, &vbo_position);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
            glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // VBO For Texcoord
            glGenBuffers(1, &vbo_texcoord);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
            glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
            glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Unbinding of vao_fire
            glBindVertexArray(0);


            return True;
        }

        void display(GLuint sphere_vao, GLsizei sphere_vertex, float time)
        {
            // code
            static float f = 0.0f;
            glUseProgram(shaderProgramObject);

            glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
            glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
            glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

            glUniform2f(resolutionUniform, (GLfloat)giWindowWidth, (GLfloat)giWindowHeight);
            glUniform1f(timeUniform, f);
            // glUniform1f(timeUniform, time);
            // printf("time = %f\n", time);
            glBindVertexArray(sphere_vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere_vertex);
            glBindVertexArray(0);

            glUseProgram(0);
            f += 0.001f;
        }


        void uninitialize(void)
        {
            UninitializeShaders(shaderProgramObject);
        }

};

#endif