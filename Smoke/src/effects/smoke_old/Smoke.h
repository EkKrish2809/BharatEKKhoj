#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "../../utils/OpenGL/Framebuffer.h"
#include "SortParticles.h"

#define NUM_EMITTERS		2
#define MAX_PARTICLES		8192
#define THREADGROUP_SIZE	256

#define EMIT_RATE			0.2f
#define LIFE_SPAN			7.0f
#define BUOYANCY			0.002f

#define BUDDHA_SCALE		25.0f
#define BOWL_SCALE			0.005f

#define BOWL1_POSITION		-2.5f, 0.0f, -0.5f
#define BOWL2_POSITION		2.5f, 0.0f, -0.5f

// sample structures
struct Particle
{
	vec4 position;
	vec4 velocity;
	vec4 color;
};

class Smoke
{
public:
    // Variables
    /* SKY RELATED VARIALBES*/
    GLuint shaderProgramObjectSmokeEmit;
    GLuint randomTexUniform;
    GLuint timeUniform;
    GLuint emitRateUniform;

    GLuint shaderProgramObjectSmokeUpdate;
    GLuint elapsedTimeUniform;
    GLuint buoyancyUniform;
    GLuint particleLifeUniform; // use for Billboard shaders as well

    GLuint shaderProgramObjectBillboard;
    GLuint sampler0Uniform;
    GLuint sampler1Uniform;
    GLuint projectMatrixUniformB; // model View Projection
    GLuint viewMatrixUniformB;    // model View Projection
    GLuint modelMatrixUniformB;   // model View Projection
    GLuint clipRadiusUniform;

    GLuint resolutionUniform;
    GLuint textureSamplerUniform;

    // FRAMEBUFFER
    FrameBuffer *smoke_buffer = NULL;
    GLuint fbo_smokeA;
    GLuint rbo_smokeA;
    GLuint fbo_texture_smokeA;

    GLuint fbo_smokeB;
    GLuint rbo_smokeB;
    GLuint fbo_texture_smokeB;

    GLuint texture_smoke;

   
    
    Bool initializeSmokeEmit(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/particle.vs", VERTEX);

        // geometry Shader
        GLuint geometryShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smokeemitter.gs", GEOMETRY);

        shaderProgramObjectSmokeEmit = glCreateProgram();
        glAttachShader(shaderProgramObjectSmokeEmit, vertexShaderObject);
        glAttachShader(shaderProgramObjectSmokeEmit, geometryShaderObject);

        glProgramParameteri(shaderProgramObjectSmokeEmit, GL_PROGRAM_SEPARABLE, GL_TRUE);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObjectSmokeEmit, 0, "particlePos");
        // Binding velocity Array
        glBindAttribLocation(shaderProgramObjectSmokeEmit, 1, "particleVel");
        // Binding color array
        glBindAttribLocation(shaderProgramObjectSmokeEmit, 2, "particleColor");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObjectSmokeEmit);
        if (bShaderLinkStatus == False)
            return False;
            
        // post link - getting
        // projectMatrixUniform = glGetUniformLocation(shaderProgramObjectSmokeEmit, "u_projectionMatrix");
        // viewMatrixUniform = glGetUniformLocation(shaderProgramObjectSmokeEmit, "u_viewMatrix");
        // modelMatrixUniform = glGetUniformLocation(shaderProgramObjectSmokeEmit, "u_modelMatrix");

        randomTexUniform = glGetUniformLocation(shaderProgramObjectSmokeEmit, "randomTex");
        timeUniform = glGetUniformLocation(shaderProgramObjectSmokeEmit, "time");
        emitRateUniform = glGetUniformLocation(shaderProgramObjectSmokeEmit, "emitRate");

        return True;
    }

    Bool initializeSmokeUpdate(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smokeupdate.vs", VERTEX);

        // geometry Shader
        GLuint geometryShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/smokeupdate.gs", GEOMETRY);

        shaderProgramObjectSmokeUpdate = glCreateProgram();
        glAttachShader(shaderProgramObjectSmokeUpdate, vertexShaderObject);
        glAttachShader(shaderProgramObjectSmokeUpdate, geometryShaderObject);

        glProgramParameteri(shaderProgramObjectSmokeUpdate, GL_PROGRAM_SEPARABLE, GL_TRUE);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObjectSmokeUpdate, 0, "particlePos");
        // // Binding velocity Array
        glBindAttribLocation(shaderProgramObjectSmokeUpdate, 1, "particleVel");
        // // Binding color Array
        glBindAttribLocation(shaderProgramObjectSmokeUpdate, 2, "particleColor");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObjectSmokeUpdate);
        if (bShaderLinkStatus == False)
            return False;
            
        // post link - getting
        // projectMatrixUniform = glGetUniformLocation(shaderProgramObjectSmokeUpdate, "u_projectionMatrix");
        // viewMatrixUniform = glGetUniformLocation(shaderProgramObjectSmokeUpdate, "u_viewMatrix");
        // modelMatrixUniform = glGetUniformLocation(shaderProgramObjectSmokeUpdate, "u_modelMatrix");

        elapsedTimeUniform = glGetUniformLocation(shaderProgramObjectSmokeUpdate, "elapsedTime");
        buoyancyUniform = glGetUniformLocation(shaderProgramObjectSmokeUpdate, "buoyancy");
        particleLifeUniform = glGetUniformLocation(shaderProgramObjectSmokeUpdate, "particleLife");

        return True;
    }

    Bool initializeBillboard(void)
    {
        // vertex Shader
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/particle.vs", VERTEX);

        // geometry Shader
        GLuint geometryShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/billboard.gs", GEOMETRY);

        // fragment Shader
        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/smoke/billboard.fs", FRAGMENT);

        shaderProgramObjectBillboard = glCreateProgram();
        glAttachShader(shaderProgramObjectBillboard, vertexShaderObject);
        glAttachShader(shaderProgramObjectBillboard, geometryShaderObject);
        glAttachShader(shaderProgramObjectBillboard, fragmentShaderObject);

        glProgramParameteri(shaderProgramObjectBillboard, GL_PROGRAM_SEPARABLE, GL_TRUE);

        // prelinked binding
        // Binding Position Array
        glBindAttribLocation(shaderProgramObjectSmokeUpdate, 0, "particlePos");
        // // Binding velocity Array
        glBindAttribLocation(shaderProgramObjectSmokeUpdate, 1, "particleVel");
        // // Binding color Array
        glBindAttribLocation(shaderProgramObjectSmokeUpdate, 2, "particleColor");

        // link
        Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObjectBillboard);
        if (bShaderLinkStatus == False)
            return False;
            
        // post link - getting
        projectMatrixUniformB = glGetUniformLocation(shaderProgramObjectBillboard, "u_projectionMatrix");
        viewMatrixUniformB = glGetUniformLocation(shaderProgramObjectBillboard, "u_viewMatrix");
        modelMatrixUniformB = glGetUniformLocation(shaderProgramObjectBillboard, "u_modelMatrix");

        sampler0Uniform = glGetUniformLocation(shaderProgramObjectBillboard, "sampler0");
        sampler1Uniform = glGetUniformLocation(shaderProgramObjectBillboard, "sampler1");
        clipRadiusUniform = glGetUniformLocation(shaderProgramObjectBillboard, "clipRadius");
        particleLifeUniform = glGetUniformLocation(shaderProgramObjectBillboard, "particleLife");

        return True;
    }

    void display(void)
    {
        // code

        
    }

    void update(void)
    {
    }

    void uninitialize(void)
    {
    }
};
