#ifndef SMOKEEFFECT_H
#define SMOKEEFFECT_H

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"



class SmokeEffect
{
    public:
        GLuint shaderProgramObjectSmoke;
        GLuint posBuf[2], velBuf[2];
        GLuint particleArray[2];
        GLuint feedback[2], initVel, startTime[2];
        GLuint drawBuf;
        GLuint renderSub, updateSub;

        int nParticles;
        float angle;
        float time, deltaT;

        SmokeEffect() : drawBuf(1), time(0), deltaT(0) {}


        Bool initialize(void)
        {
            // code
            // vertex Shader
            GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smoke.vs", VERTEX);

            // fragment Shader
            GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smoke.fs", FRAGMENT);

            shaderProgramObjectSmoke = glCreateProgram();
            glAttachShader(shaderProgramObjectSmoke, vertexShaderObject);
            glAttachShader(shaderProgramObjectSmoke, fragmentShaderObject);

            glProgramParameteri(shaderProgramObjectSmoke, GL_PROGRAM_SEPARABLE, GL_TRUE);

            // prelinked binding
            // Binding Position Array
            glBindAttribLocation(shaderProgramObjectSmoke, 0, "particlePos");
            // Binding velocity Array
            glBindAttribLocation(shaderProgramObjectSmoke, 1, "particleVel");
            // Binding color array
            glBindAttribLocation(shaderProgramObjectSmoke, 2, "particleColor");


            const char * outputNames[] = { "Position", "Velocity", "StartTime" };
            glTransformFeedbackVaryings(shaderProgramObjectSmoke, 3, outputNames, GL_SEPARATE_ATTRIBS);
            // link
            Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObjectSmoke);
            if (bShaderLinkStatus == False)
                return False;

            // use program
            glUseProgram(shaderProgramObjectSmoke);
            
            renderSub = glGetSubroutineIndex(shaderProgramObjectSmoke, GL_VERTEX_SHADER, "render");
            updateSub = glGetSubroutineIndex(shaderProgramObjectSmoke, GL_VERTEX_SHADER, "update");

            glClearColor(1.0f,1.0f,1.0f,1.0f);

            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(10.0);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            modelMatrix = mat4::identity();


            
            return True;
        }

        float randFloatF() {
            return ((float)rand() / RAND_MAX);
        }

        void initBuffers()
        {
            nParticles = 1000;

            // Generate the buffers
            glGenBuffers(2, posBuf);    // position buffers
            glGenBuffers(2, velBuf);    // velocity buffers
            glGenBuffers(2, startTime); // Start time buffers
            glGenBuffers(1, &initVel);  // Initial velocity buffer (never changes, only need one)

            // Allocate space for all buffers
            int size = nParticles * 3 * sizeof(float);
            glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, initVel);
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
            glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);
            glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
            glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_DYNAMIC_COPY);

            // Fill the first position buffer with zeroes
            GLfloat *data = new GLfloat[nParticles * 3];
            for( int i = 0; i < nParticles * 3; i++ ) data[i] = 0.0f;
            glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

            // Fill the first velocity buffer with random velocities
            float theta, phi, velocity;
            vec3 v(0.0f);
            for( int i = 0; i < nParticles; i++ ) {
                // theta = glm::mix(0.0f, glm::pi<float>() / 1.5f, randFloat());
                // phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());
                theta = vmath::mix(0.0f, vmath::pi<float>() / 1.5f, randFloatF());
                phi = vmath::mix(0.0f, vmath::two_pi<float>(), randFloatFz());

                v.x = sinf(theta) * cosf(phi);
                v.y = cosf(theta);
                v.z = sinf(theta) * sinf(phi);

                velocity = glm::mix(0.1f,0.2f,randFloat());
                v = glm::normalize(v) * velocity;

                data[3*i]   = v.x;
                data[3*i+1] = v.y;
                data[3*i+2] = v.z;
            }
            glBindBuffer(GL_ARRAY_BUFFER,velBuf[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
            glBindBuffer(GL_ARRAY_BUFFER,initVel);
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

            // Fill the first start time buffer
            delete [] data;
            data = new GLfloat[nParticles];
            float time = 0.0f;
            float rate = 0.01f;
            for( int i = 0; i < nParticles; i++ ) {
                data[i] = time;
                time += rate;
            }
            glBindBuffer(GL_ARRAY_BUFFER,startTime[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

            glBindBuffer(GL_ARRAY_BUFFER,0);
            delete [] data;

            // Create vertex arrays for each set of buffers
            glGenVertexArrays(2, particleArray);

            // Set up particle array 0
            glBindVertexArray(particleArray[0]);
            glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, startTime[0]);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, initVel);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(3);

            // Set up particle array 1
            glBindVertexArray(particleArray[1]);
            glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, startTime[1]);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, initVel);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(3);

            glBindVertexArray(0);

            // Setup the feedback objects
            glGenTransformFeedbacks(2, feedback);

            // Transform feedback 0
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[0]);

            // Transform feedback 1
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, startTime[1]);

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        }


        void display(void)
        {
            // code
        }

        void undate(void)
        {
            // code
        }

        void uninitialize(void)
        {
            // code
        }
};

#endif
