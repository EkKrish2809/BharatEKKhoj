#pragma once
#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/Camera.h"
// #include "../../includes/3DModels.h"
// #include "../../OGLMain.h"
// #include "../../shaders/model/Model_Shader.h"
// #include "../../scenes/01-Opening/Opening.h"

#include "../../utils/common.h"

extern Camera camera;

class Shadow
{
public:
	// Opening opening;
	// JokerFace jokerface;
	// ModelShader joker_model;

	// Variables
	GLuint shaderProgramObject_depth;
	GLuint shaderProgramObject_shadow;
	GLuint shaderProgramObject_debugQuad;

	GLuint modelMatrixUniform_depth;
	GLuint viewMatrixUniform_depth;
	GLuint projectionMatrixUniform_depth;

	GLuint modelMatrixUniform_shadow;
	GLuint viewMatrixUniform_shadow;
	GLuint projectionMatrixUniform_shadow;
	GLuint lightSpaceMatrixUniform_shadow;
	GLuint diffuseTextureUniform_shadow;
	GLuint shadowMapTextureUniform_shadow;
	GLuint lightPosUniform_shadow;
	GLuint viewPosUniform_shadow;

	GLuint depthMapTextureUniform_debudQuad;
	GLuint nearPlaneUniform_debudQuad;
	GLuint farPlaneUniform_debudQuad;

	GLuint woodTexture = 0;

	GLint giWidth;
	GLint giHeight;

	GLuint planeVAO;
	GLuint planeVBO;

	GLuint quadVAO = 0;
	GLuint quadVBO;

	GLuint cubeVAO = 0;
	GLuint cubeVBO = 0;

	GLuint depthMapFBO;
	GLuint depthMapTexture;

	float lightPos[3] = {0.0f, 0.0f, 2.0f};
	// GLfloat lightPos[3];
	// lightPos[0] = 0.0f;
	// lightPos[1] = 0.0f;
	// lightPos[2] = 1.0f;

	// GLfloat objIncrement = 1.0f;
	// GLfloat objX = 0.0f, objY = 0.0f, objZ = 0.0f;
	// GLfloat light_objX = 0.0f, light_objY = 0.0f, light_objZ = 0.0f;

	// Cube *cube = nullptr;

	GLuint SHADOW_WIDTH = 1024;
	GLuint SHADOW_HEIGHT = 1024;

	Bool initialize()
	{

		// BOOL status = opening.initialize();
		// if(status == FALSE)
		//{
		//     PrintLog("joker model loading failed\n");
		// }

		// BOOL status = joker_model.initialize_ModelShaderObject(".\\assets\\models\\Joker\\JOKER FACE\\JOKER.obj");
		// if (status == FALSE)
		// {
		// 	PrintLog("joker model loading failed\n");
		// }

		// vertex Shader
		GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/shadow/shadow_mapping_depth.vs", VERTEX);

		// fragment Shader
		GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/shadow/shadow_mapping_depth.fs", FRAGMENT);

		shaderProgramObject_depth = glCreateProgram();
		glAttachShader(shaderProgramObject_depth, vertexShaderObject);
		glAttachShader(shaderProgramObject_depth, fragmentShaderObject);

		// prelinked binding
		// Binding Position Array
		glBindAttribLocation(shaderProgramObject_depth, MATRIX_ATTRIBUTE_POSITION, "a_position");
		// Binding Color Array
		glBindAttribLocation(shaderProgramObject_depth, MATRIX_ATTRIBUTE_NORMAL, "a_normal");

		// link
		Bool bShaderLinkStatus = LinkShaderProgramObject(shaderProgramObject_depth);

		if (bShaderLinkStatus == False)
			return False;
		// post link - getting
		modelMatrixUniform_depth = glGetUniformLocation(shaderProgramObject_depth, "u_modelMatrix");
		// viewMatrixUniform_depth = glGetUniformLocation(shaderProgramObject_depth, "u_viewMatrix");
		projectionMatrixUniform_depth = glGetUniformLocation(shaderProgramObject_depth, "u_lightSpaceMatrix");

		// Declaration of vertex data array
		float planeVertices[] = {
			// positions            // normals         // texcoords
			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

			25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f};
		// plane VAO

		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_NORMAL);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glBindVertexArray(0);

		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,	// top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,	// top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	  // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// top-right
																// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	// top-right
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,		// top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,	// bottom-left
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,	// top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,	// bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,	  // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f	  // bottom-left
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_NORMAL);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,
			1.0f,
			0.0f,
			0.0f,
			1.0f,
			-1.0f,
			-1.0f,
			0.0f,
			0.0f,
			0.0f,
			1.0f,
			1.0f,
			0.0f,
			1.0f,
			1.0f,
			1.0f,
			-1.0f,
			0.0f,
			1.0f,
			0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_POSITION);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(MATRIX_ATTRIBUTE_TEXTURE0);
		glVertexAttribPointer(MATRIX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Shadow mapping shader
		if (InitializeShadowMapping() == False)
		{
			printf("Failed :: InitializeShadowMapping() \n");
			return False;
		}

		// Debug quad
		if (initializeDebugQuad() == False)
		{
			printf("Failed :: initializeDebugQuad() \n");
			return False;
		}

		// loading textures
		if (LoadGlTexture(&woodTexture, "./assets/textures/fire/fire.png") == False)
		{
			return (-7);
		}

		// Create Depth FBO
		createFBO();

		return True;
	}
	// Shaders for ShadowMap
	Bool InitializeShadowMapping(void)
	{
		GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/shadow/shadow_mapping.vs", VERTEX);
		if (vertexShaderObject == 0)
		{
			printf("Failed to compile VertexShader for ShadowMap\n");
			uninitialize();
			return False;
		}

		GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/shadow/shadow_mapping.fs", FRAGMENT);
		if (fragmentShaderObject == 0)
		{
			printf("Failed to compile FragmentShader for ShadowMap\n");
			uninitialize();
			return False;
		}
		// shader program object
		shaderProgramObject_shadow = glCreateProgram();

		glAttachShader(shaderProgramObject_shadow, vertexShaderObject);
		glAttachShader(shaderProgramObject_shadow, fragmentShaderObject);

		// prelinked binding
		glBindAttribLocation(shaderProgramObject_shadow, MATRIX_ATTRIBUTE_POSITION, "aPos");
		glBindAttribLocation(shaderProgramObject_shadow, MATRIX_ATTRIBUTE_TEXTURE0, "aTexCoords");
		glBindAttribLocation(shaderProgramObject_shadow, MATRIX_ATTRIBUTE_NORMAL, "aNormal");

		// Linking the shaderProgramObject_shadow
		if (LinkShaderProgramObject(shaderProgramObject_shadow) == False)
		{
			printf("Failed to link SHadow_Mapping\n");
			return False;
		}
		//
		modelMatrixUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "u_modelMatrix");
		viewMatrixUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "u_viewMatrix");
		projectionMatrixUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "u_projectionMatrix");
		lightSpaceMatrixUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "u_lightSpaceMatrix");

		diffuseTextureUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "diffuseTexture");
		shadowMapTextureUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "shadowMap");

		lightPosUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "lightPos");
		viewPosUniform_shadow = glGetUniformLocation(shaderProgramObject_shadow, "viewPos");

		return True;
	}

	// Shaders for Debug Quad
	Bool initializeDebugQuad(void)
	{
		GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/shadow/debug_quad.vs", VERTEX);
		if (vertexShaderObject == 0)
		{
			printf("Failed to compile VertexShader for Debug Quad\n");
			uninitialize();
			return False;
		}

		GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/shadow/debug_quad.fs", FRAGMENT);
		if (fragmentShaderObject == 0)
		{
			printf("Failed to compile FragmentShader for Debug Quad\n");
			uninitialize();
			return False;
		}
		// shader program object
		shaderProgramObject_debugQuad = glCreateProgram();

		glAttachShader(shaderProgramObject_debugQuad, vertexShaderObject);
		glAttachShader(shaderProgramObject_debugQuad, fragmentShaderObject);

		// prelinked binding
		glBindAttribLocation(shaderProgramObject_debugQuad, MATRIX_ATTRIBUTE_POSITION, "aPos");
		glBindAttribLocation(shaderProgramObject_debugQuad, MATRIX_ATTRIBUTE_TEXTURE0, "aTexCoords");

		// Linking the shaderProgramObject_debugQuad
		if (LinkShaderProgramObject(shaderProgramObject_debugQuad) == False)
		{
			printf("Failed to link Debug Quad\n");
			return False;
		}
		//
		depthMapTextureUniform_debudQuad = glGetUniformLocation(shaderProgramObject_debugQuad, "depthMap");
		nearPlaneUniform_debudQuad = glGetUniformLocation(shaderProgramObject_debugQuad, "near_plane");
		farPlaneUniform_debudQuad = glGetUniformLocation(shaderProgramObject_debugQuad, "far_plane");

		return True;
	}

		

		

		
	

	void createFBO(void)
	{
		// Create FBO
		glGenFramebuffers(1, &depthMapFBO);

		// create depth texture
		glGenTextures(1, &depthMapTexture);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float borderColor[] = {1.0, 1.0, 1.0, 1.0};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
		glDrawBuffer(GL_NONE);
		// glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			PrintLog("Framebuffer Incomplete\n");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		PrintLog("----------  exiting CreateFBO  -----------------\n");
	}

	void renderQuad()
	{
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		PrintLog("----------  exiting renderQuad  -----------------\n");
	}

	void renderCube()
	{
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// joker_model.render_Model();
		// opening.display();
		// printf("----------  exiting renderCube  -----------------\n");
	}

	void renderScene(GLuint modelMatrixUniformParam, GLuint _shaderProgramObject, Bool isDepthMap, GLuint depthMapTexture)
	{
		viewMatrix = camera.getViewMatrix();
		// floor
		pushMatrix(modelMatrix);
		{
			mat4 modelMatrix1 = mat4::identity();
			mat4 translateMatrix = mat4::identity();
			translateMatrix = vmath::translate(0.0f, 0.0f, 0.0f);
			modelMatrix = modelMatrix * translateMatrix;
			glUniformMatrix4fv(modelMatrixUniformParam, 1, GL_FALSE, modelMatrix);

			glBindVertexArray(planeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
		modelMatrix = popMatrix();

		// cubes
		pushMatrix(modelMatrix);
		{
			mat4 modelMatrix1 = mat4::identity();
			mat4 translateMatrix = mat4::identity();
			mat4 scaleMatrix = mat4::identity();
			translateMatrix = vmath::translate(0.0f, 1.5f, 0.0f);
			scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);
			modelMatrix = modelMatrix * translateMatrix * scaleMatrix;
			glUniformMatrix4fv(modelMatrixUniformParam, 1, GL_FALSE, modelMatrix);
			renderCube();
			// opening.display(shaderProgramObject);
			// joker_model.render_Model_shadow(_shaderProgramObject, isDepthMap, depthMapTexture);
		}
		modelMatrix = popMatrix();

		pushMatrix(modelMatrix);
		{
			mat4 modelMatrix1 = mat4::identity();
			mat4 translateMatrix = mat4::identity();
			mat4 scaleMatrix = mat4::identity();
			translateMatrix = vmath::translate(2.0f, 0.0f, 1.0f);
			scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);
			modelMatrix = modelMatrix * translateMatrix * scaleMatrix;
			glUniformMatrix4fv(modelMatrixUniformParam, 1, GL_FALSE, modelMatrix);
			renderCube();
			// opening.display(shaderProgramObject);
			// joker_model.render_Model_shadow(_shaderProgramObject, isDepthMap, depthMapTexture);
		}
		modelMatrix = popMatrix();

		pushMatrix(modelMatrix);
		{
			mat4 modelMatrix1 = mat4::identity();
			mat4 translateMatrix = mat4::identity();
			mat4 scaleMatrix = mat4::identity();
			mat4 rotationMatrix = mat4::identity();
			translateMatrix = vmath::translate(-1.0f, 0.0f, 2.0f);
			scaleMatrix = vmath::scale(0.25f, 0.25f, 0.25f);
			rotationMatrix = vmath::rotate(60.0f, 1.0f, 0.0f, 1.0f);
			modelMatrix = modelMatrix * translateMatrix * rotationMatrix * scaleMatrix;
			glUniformMatrix4fv(modelMatrixUniformParam, 1, GL_FALSE, modelMatrix);
			renderCube();
			// opening.display(shaderProgramObject);
			// joker_model.render_Model_shadow(_shaderProgramObject, isDepthMap, depthMapTexture);
		}
		modelMatrix = popMatrix();
	}

	void renderMainScene(void)
	{
		// Function declarations
		// void renderScene(GLuint);
		// void renderQuad(void);

		mat4 lightProjectionMatrix = mat4::identity();
		mat4 lightViewMatrix = mat4::identity();
		mat4 lightSpaceMatrix = mat4::identity();
		mat4 translateMatrix = mat4::identity();
		float near_plane = 1.0f, far_plane = 7.5f * 4;

		// lightPos[0] = light_objX;
		// lightPos[1] = light_objY;
		// lightPos[2] = light_objZ;

		// lightPos[0] = opening.jokerFace.lightPosition[0] + light_objX;
		// lightPos[1] = opening.jokerFace.lightPosition[1] + light_objY;
		// lightPos[2] = opening.jokerFace.lightPosition[2] + light_objZ;

		lightPos[0] = objX;
		lightPos[1] = objY;
		lightPos[2] = objZ;

		printf("lightPos[0] = %f, lightPos[1] = %f, lightPos[2] = %f\n", lightPos[0], lightPos[1], lightPos[2]);

		// lightProjectionMatrix = vmath::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightProjectionMatrix = vmath::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
		lightViewMatrix = vmath::lookat(vec3(lightPos[0], lightPos[1], lightPos[2]), vec3(0.0f), vec3(0.0, 1.0, 0.0));
		// lightViewMatrix = vmath::lookat(vec3(-2.0f, 4.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

		pushMatrix(modelMatrix);
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Use Depth Shader
			glUseProgram(shaderProgramObject_depth);
			{
				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

				glUniformMatrix4fv(projectionMatrixUniform_depth, 1, GL_FALSE, lightSpaceMatrix);
				// glUniformMatrix4fv(viewMatrixUniform_depth, 1, GL_FALSE, lightViewMatrix);
				glClear(GL_DEPTH_BUFFER_BIT);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, woodTexture);
				renderScene(modelMatrixUniform_depth, shaderProgramObject_depth, True, 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			glUseProgram(0);

			// // reset viewport
			glViewport(0, 0, giWindowWidth, giWindowHeight);
			// PrintLog("giWindowWidth = %d, giWindowHeight = %d\n", giWindowWidth, giWindowHeight);
			// // glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// // glCullFace(GL_FRONT);

			// Use default shader + shadow Shader
			glUseProgram(shaderProgramObject_shadow);
			{
				mat4 viewMatrix1 = mat4::identity();
				// viewMatrix1 = viewMatrix * vmath::lookat(vec3(0.0f + objX, 0.0f + objY, 2.0f + objZ), vec3(0.0f, 0.0f, -5.0f), vec3(0.0f, 1.0f, 0.0f));
				viewMatrix1 = camera.getViewMatrix();
				// vec3 viewPos = vec3(0.0f + objX, 0.0f + objY, 2.0f + objZ);
				vec3 viewPos = camera.getEye();

				glUniformMatrix4fv(projectionMatrixUniform_shadow, 1, GL_FALSE, perspectiveProjectionMatrix);
				glUniformMatrix4fv(viewMatrixUniform_shadow, 1, GL_FALSE, viewMatrix1);
				glUniformMatrix4fv(lightSpaceMatrixUniform_shadow, 1, GL_FALSE, lightSpaceMatrix);
				glUniform3fv(viewPosUniform_shadow, 1, viewPos);
				glUniform3fv(lightPosUniform_shadow, 1, lightPos);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, woodTexture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, depthMapTexture);
				glUniform1i(diffuseTextureUniform_shadow, 0);
				glUniform1i(shadowMapTextureUniform_shadow, 1);
				renderScene(modelMatrixUniform_shadow, shaderProgramObject_shadow, False, depthMapTexture);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glUseProgram(0);
			// glCullFace(GL_BACK);

			glCullFace(GL_FRONT);
			glUseProgram(shaderProgramObject_debugQuad);
			glUniform1f(nearPlaneUniform_debudQuad, near_plane);
			glUniform1f(farPlaneUniform_debudQuad, far_plane);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthMapTexture);
			glUniform1i(depthMapTextureUniform_debudQuad, 0);
			// renderQuad();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUseProgram(0);
			glCullFace(GL_BACK);
		}
		modelMatrix = popMatrix();

		// For denominating light position
		// pushMatrix(modelMatrix);
		//{
		//	translateMatrix = vmath::translate(lightPos[0], lightPos[1], lightPos[2]);
		//	modelMatrix = modelMatrix * translateMatrix;
		//	renderCube();
		//}
		// modelMatrix = popMatrix();
	}

	void display(void)
	{
		// Function declaration
		// void renderMainScene(void);
		// void renderCube(void);

		// Call Function
		// renderMainScene();

		// lightPos[0] = light_objX;
		// lightPos[1] = light_objY;
		// lightPos[2] = light_objZ;
		//
		// viewMatrix = camera.getViewMatrix();
		renderMainScene();
	}

	void update(void)
	{
		// code
		// if(selected_scene == SCENE_OPENING)
		// opening.update();
	}

	void uninitialize(void)
	{
		if (planeVBO)
		{
			glDeleteBuffers(1, &planeVBO);
			planeVBO = 0;
		}

		// Deletion/uninitialization of VAO
		if (planeVAO)
		{
			glDeleteVertexArrays(1, &planeVAO);
			planeVAO = 0;
		}

		UninitializeShaders(shaderProgramObject_debugQuad);
		UninitializeShaders(shaderProgramObject_shadow);
		UninitializeShaders(shaderProgramObject_depth);
		// opening.uninitialize(/
	}
};