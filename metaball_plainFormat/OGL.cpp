// Standard Header files
#include <stdio.h> // for standard I/O
#include <stdlib.h> // for exit()
#include <memory.h> // for memset()

// X11 header (11 th version of 'X') 
#include <X11/Xlib.h> 
#include <X11/Xutil.h> // XVisualInfo
#include <X11/XKBlib.h> // for KeyBoard

// OpenGL header files
#include <GL/glew.h> // 
#include <GL/gl.h> // for OpenGL functionality
#include <GL/glx.h> // for bridging API's

// Texture header
#include <SOIL/SOIL.h>

#include "vmath.h"
using namespace vmath;

mat4 perspectiveProjectionMatrix;

#include "MetaBall.h"



// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global variables
Display *display = NULL;
XVisualInfo *visualInfo = NULL;
Colormap colorMap;
Window window;
Bool fullScreen = False;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;

GLXContext glxContext; // OpenGL related
Bool bActiceWindow = False;

FILE *gpFile = NULL;

GLuint shaderProgramObject;

enum
{
	KPE_ATTRIBUTE_POSITION = 0,
	KPE_ATTRIBUTE_COLOR,
	KPE_ATTRIBUTE_NORMAL,
	KPE_ATTRIBUTE_TEXTURE0
};

GLuint vao;
GLuint vbo_position;
GLuint vbo_texcoord;

GLuint texture_smiley;

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform;



// Entry point function
int main(void)
{
	// function declarations
	void toggleFullscreen(void);
	int initialize(void);
	void resize(int, int);
	void draw(void);
	void uninitialize(void);

	// local variables
	int defaultScreen;
	int defaultDepth;
	// PP variables
	GLXFBConfig *glxFBConfigs = NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *tmpXVisualInfo = NULL;
	int numFBConfig;

	XSetWindowAttributes windowAttribute;
	int styleMask;
	Atom wm_delete_window_atom;
	XEvent event;
	KeySym keysym;
	int screenWidth;
	int screenHeight;
	char keys[26];

	static int frameBufferAttributes[] = 
			{
				GLX_DOUBLEBUFFER, True,
				GLX_X_RENDERABLE, True, // context becomes hardware renderable
				GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, // where we have to draw -> on window (GLX_WINDOW_BIT)
				GLX_RENDER_TYPE, GLX_RGBA_BIT, // RGBA type image (frame)
				GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
				GLX_RED_SIZE, 8,
				GLX_GREEN_SIZE, 8,
				GLX_BLUE_SIZE, 8,
				GLX_ALPHA_SIZE, 8,
				GLX_STENCIL_SIZE, 8,
				GLX_DEPTH_SIZE, 24,
				None
			}; // like pfd in Windows OS
	Bool bDone = False;
	static int win_width, win_height;
	

	// code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		printf("Creation of Log file failed. Exitting...\n");
	}

	// display struct is availed by XServer to us
	display = XOpenDisplay(NULL);
	if (display == NULL)	
	{
		fprintf(gpFile,"ERROR : XOpenDisplay() Failed !!!\n");
		uninitialize();
		exit(1);
	}

	//
	defaultScreen = XDefaultScreen(display);

	// 
	defaultDepth = XDefaultDepth(display, defaultScreen);

	// choosing best visualInfo
	glxFBConfigs = glXChooseFBConfig(display, defaultScreen, frameBufferAttributes, &numFBConfig); // returns array of FB configs and its count
	if (glxFBConfigs == NULL)
	{
		fprintf(gpFile, "ERROR : glXChooseFBConfig() Failed\n");
		uninitialize();
		exit(1);
	}
	fprintf(gpFile, "Found number of FBConfigs are %d\n", numFBConfig);

	// find best FBConfig from the aray
	int bestFrameBufferConfig = -1;
	int worstFrameBufferConfig = -1;
	int bestNumberOfSamples = -1;
	int worstNumberOfSamples = 999;

	for (int i = 0; i < numFBConfig; i++)
	{
		tmpXVisualInfo = glXGetVisualFromFBConfig(display, glxFBConfigs[i]);
		if (tmpXVisualInfo != NULL)
		{
			int samples, sampleBuffers;
			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLES, &samples);
	
			fprintf(gpFile, "VisualInfo = 0x%lu found SampleBuffers = %d and samples = %d\n", tmpXVisualInfo->visualid, sampleBuffers, samples);

			if (bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}

			if (worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(tmpXVisualInfo);
		tmpXVisualInfo = NULL;
	}
	
	bestGLXFBConfig = glxFBConfigs[bestFrameBufferConfig];
	glxFBConfig = bestGLXFBConfig;
	XFree(glxFBConfigs);
	glxFBConfigs = NULL;

	visualInfo = glXGetVisualFromFBConfig(display, bestGLXFBConfig);
	fprintf(gpFile, "visualId of bestVisualInfo is 0x%lu\n", visualInfo->visualid);


	// not in PP
/*	visualInfo = glXChooseVisual(display, defaultScreen, frameBufferAttributes); // frmaebuffer shi match karnara visualinfo choose kar
	if (visualInfo == NULL)
	{
		fprintf(gpFile,"ERROR : glXChooseVisual() Failed !!!\n");
		uninitialize();
		exit(1);
	}
*/
	// 
	memset(&windowAttribute, 0, sizeof(XSetWindowAttributes));
	windowAttribute.border_pixel = 0;
	windowAttribute.background_pixel = XBlackPixel(display, defaultScreen);
	windowAttribute.background_pixmap = 0; // bkg picture vapraychay ka asa meaning hoto
	windowAttribute.colormap = XCreateColormap(display, 
						RootWindow(display, visualInfo->screen),
						visualInfo->visual,
						AllocNone);
	windowAttribute.event_mask = ExposureMask | KeyPressMask| StructureNotifyMask | FocusChangeMask; //

	// 
	colorMap = windowAttribute.colormap;

	//
	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

	//
	window = XCreateWindow(display,
			RootWindow(display, visualInfo->screen),
			100, // x
			100, // y
			WIN_WIDTH, // width
			WIN_HEIGHT, // height
			0, // border width
			visualInfo->depth,  // depth of window
			InputOutput,
			visualInfo->visual,
			styleMask,
			&windowAttribute); 
	if (!window)
	{
		fprintf(gpFile,"ERROR : XCreateWindow() Failed !!!\n");
		uninitialize();
		exit(1);
	}
	
	//
	XStoreName(display, window, "KPE : OpenGL-XWindows");
	
	// for closing window ... Xwindow does not have close button by default, 
	// we ask for it to Window Manager(GNOME, KDE, etc)
	wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", True); // WM -> Window Manager

	XSetWMProtocols(display, window, &wm_delete_window_atom, 1);

	// showing window
	XMapWindow(display, window);

	// centering of Window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(display, defaultScreen)); 
	screenHeight = XHeightOfScreen(XScreenOfDisplay(display, defaultScreen));
	XMoveWindow(display, window, screenWidth/2 - WIN_WIDTH/2, screenHeight/2 - WIN_HEIGHT/2);

	// call to initialize
	initialize();

	// message loop

	while(bDone == False)
	{
			while (XPending(display)) // analogus to PeekMessage() in Windows OS
                	{
                		XNextEvent(display, &event); // GetMessage()
                		switch (event.type)
                		{
                			case MapNotify: // WM_CREATE in windows
                				break;
					case FocusIn:
						bActiceWindow = True;
						break;
					case FocusOut:
						bActiceWindow = False;
						break;
                			case KeyPress:
                				keysym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
                				switch (keysym)
                				{
                					case XK_Escape:
								bDone = True;
                						break;
                				}
                
                				XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
                				switch (keys[0])
                				{
                					case 'F':
                					case 'f':
                						if (fullScreen == False)
                						{
                							toggleFullscreen();
                							fullScreen = True;
                						}
                						else
                						{
                							toggleFullscreen();
                							fullScreen = False;
                						}
                						break;
                				}
                				break;
					case ConfigureNotify:
						win_width = event.xconfigure.width;
						win_height = event.xconfigure.height;
						resize(win_width, win_height);
						break;
                			case 33: // retuened by WM_DELETE_WINDOW_ATOM i.e. close button
						bDone = True;
                				break;
                
                		}
                	}
			if (bActiceWindow == True)
			{
				draw();
			}
                
	}
	// 
	uninitialize();

	return(0);
}

void toggleFullscreen(void)
{
	// local variables declarations
	Atom wm_current_state_atom;
	Atom wm_fullscreen_state_atom;
	XEvent event;

	// code
	wm_current_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
	wm_fullscreen_state_atom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	memset(&event, 0, sizeof(XEvent));
	event.type = ClientMessage;
	event.xclient.window = window;
	event.xclient.message_type = wm_current_state_atom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = fullScreen ? 0 : 1;
	event.xclient.data.l[1] = wm_fullscreen_state_atom;

	// like sent and post messages in Windows OS
	XSendEvent(display,
			RootWindow(display, visualInfo->screen),
			False, // if this msg is for this window or for its child window
			SubstructureNotifyMask,
			&event);
}

int initialize(void)
{
	// function declarations
	void resize(int, int);
	void printGLInfo(void);
	Bool loadGlTexture(GLuint *texture, const char *path);
	void uninitialize(void);

	// code
	// removing this CreateContext call for PP
//	glxContext = glXCreateContext(display, visualInfo, NULL, True);

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB == NULL)
	{
		fprintf(gpFile, "glXGetProcAddressARB() Failed\n");
		uninitialize();
		exit(1);
	}

	// version = 4.5, then 4 is major version and 5 is minor version
	GLint contextAttributes[] = {
				GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
				GLX_CONTEXT_MINOR_VERSION_ARB, 5,
				GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
				None
	};

	// fallback
	glxContext = glXCreateContextAttribsARB(display, glxFBConfig, NULL, True, contextAttributes);
	if (!glxContext)
	{
	
		GLint contextAttributes[] = {
                			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
                			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                			None
                };
		glxContext = glXCreateContextAttribsARB(display, glxFBConfig, NULL, True, contextAttributes);

		fprintf(gpFile, "Cannot support 4.5, and hence falling back to default varsion\n");

	}
	else
	{
		fprintf(gpFile, "Found the Support to 4.5 version\n");
	}

	// checking if Hardware rendering
	if (!glXIsDirect(display, glxContext))
	{
		fprintf(gpFile, "Direct Rendering i.e. H.W. rendering not supported\n");
	}
	else
	{
		fprintf(gpFile, "Direct Rendering i.e. H.W. rendering supported\n");
	
	
	}

	// make it as current context
	glXMakeCurrent(display, window, glxContext);

	// GLEW initialization
        if (glewInit() != GLEW_OK)
        {
        	return(-5);
        }
                                                                                                        
        // Print OpenGL info
        printGLInfo();
                                                                                                        
        // vertex shader
        const GLchar *vertexShaderSourcecode = 
        "#version 450 core" \
        "\n" \
	"in vec4 a_position;" \
	"in vec2 a_texcoord;" \
	"uniform mat4 u_mvpMatrix;" \
	"out vec2 a_texcoord_out;" \
        "void main(void)" \
        "{" \
	"gl_Position = u_mvpMatrix * a_position;" \
	"a_texcoord_out = a_texcoord;" \
        "}";
                                                                                                        
        GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
                                                                                                        
        glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourcecode, NULL);
        // for 4th para 'NULL' -> length of array of each source code's string's length (yach array)
                                                                                                        
        glCompileShader(vertexShaderObject);
                                                                                                        
        GLint status;
        GLint infoLogLength;
        char *log = NULL;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
        	glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        	if (infoLogLength > 0)
        	{
        		log = (char *)malloc(infoLogLength);
        		if (log != NULL)
        		{
        			GLsizei written;
        			glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
        			fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", log);
        			free(log);
        			uninitialize();
				exit(0);
        		}
        	}
        }
                                                                                                        
        // Fragment Shader
        const GLchar *fragmentShaderSourcecode = 
        "#version 450 core" \
        "\n" \
	"in vec2 a_texcoord_out;" \
	"uniform sampler2D u_texturesampler;" \
	"out vec4 FragColor;" \
        "void main(void)" \
        "{" \
	"FragColor = texture(u_texturesampler, a_texcoord_out);" \
        "}";
                                                                                                        
        GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
                                                                                                        
        glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourcecode, NULL);
                                                                                                        
        glCompileShader(fragmentShaderObject);
                                                                                                        
        status = 0;
        infoLogLength = 0;
        log = NULL;
        glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
        	glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        	if (infoLogLength > 0)
        	{
        		log = (char *)malloc(infoLogLength);
        		if (log != NULL)
        		{
        			GLsizei written;
        			glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
        			fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", log);
        			free(log);
        			uninitialize();
				exit(0);
        		}
        	}
        }
                                                                                                        
        // shader program object
        shaderProgramObject = glCreateProgram();
                                                                                                        
        glAttachShader(shaderProgramObject, vertexShaderObject);
        glAttachShader(shaderProgramObject, fragmentShaderObject);
        
	// prelinked binding
	glBindAttribLocation(shaderProgramObject, KPE_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject, KPE_ATTRIBUTE_TEXTURE0, "a_texcoord");
	
	glLinkProgram(shaderProgramObject);
                                                                                                        
        status = 0;
        infoLogLength = 0;
        log = NULL;
        glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
        	glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        	if (infoLogLength > 0)
        	{
        		log = (char *)malloc(infoLogLength);
        		if (log != NULL)
        		{
        			GLsizei written;
        			glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
        			fprintf(gpFile, "Shader Program Link Log : %s\n", log);
        			free(log);
        			uninitialize();
				exit(0);
        		}
        	}
	}                                                                              

	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_texturesampler");

	// declaring vertex data array
	const GLfloat squareVertices[] = 
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

	// vao and vbo_position related changes
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	/* Below 5 'vbo_position' steps are reorded in 'vao'*/

	// recording started
	glGenBuffers(1, &vbo_position); /* 1st para -> no of buffers, 2nd para -> address from GPU */
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position); /* 1st para -> type of buffer to which vbo_position is to be bind, 2nd para -> vbo_position*/

	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW); /* 1st para -> data bharnyachi jaga, 2nd para ->size of data, 3rd para -> actual data, 4th para -> how to fill the data (statically)*/

	glVertexAttribPointer(KPE_ATTRIBUTE_POSITION, // fill the data at 0th position in shader
			3, // read the data as set of 3
			GL_FLOAT, // Data type of the data in array in GPU context
			GL_FALSE, // if TRUE, we have to give the data in range '1 to -1' i.e. normalized data
			0, // dhanga takaychi garaj nahi , karan fakt position cha array ahe (stride)
			NULL //dhanga astil tr data kuthay he sangaych ast i.e. data interleaved asel tr
			);

	glEnableVertexAttribArray(KPE_ATTRIBUTE_POSITION);/* enabling the 0th position in shader, i.e. enabling the array mapped to that position*/

	glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo_position

	// vbo for texture
	glGenBuffers(1, &vbo_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(KPE_ATTRIBUTE_TEXTURE0,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			NULL);
	glEnableVertexAttribArray(KPE_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // unbind vao

	// metaball initialize
	setupGraphics(WIN_WIDTH, WIN_HEIGHT);

       	// here starts OpenGL code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	if (loadGlTexture(&texture_smiley,"Smiley.bmp") == False)
        {
        	return(-1);
        }

	perspectiveProjectionMatrix = mat4::identity();

	// warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);
	return(0);
}

Bool loadGlTexture(GLuint *texture, const char *path)
{
	// variable declarations
	int width, height;
	unsigned char *imageData = NULL;

	// code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
	if (imageData == NULL)
	{
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

void printGLInfo(void)
{
	// local variable declarations
	GLint numExtensions = 0;

	// code
	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "Number Of Supported Extension : %d\n", numExtensions);

	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;	// to avoid divided 0(Illeegal Instruction) in future cause

	glViewport(0, 0, width, height); // screen cha konta bhag dakhvu... sadhya sagli screen dakhvli jat ahe pn he game need nusar customize karta yet.. eth width ai height he GL_SIZEI ya type che ahet ...resize la yetana te int ya type madhe yetat
	
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // initialize madhe sangitlele glClearColor che painting actually eth hot.. Frame buffer == COLOR_BUFFER -> true
	
	// use the shaderProgramObject
	// glUseProgram(shaderProgramObject);
        
	// // transformations
	// mat4 translationMatrix = mat4::identity();
	// mat4 modelViewMatrix = mat4::identity();
	// mat4 modelViewProjectionMatrix = mat4::identity();
	                                                                              
	// translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	// modelViewMatrix = translationMatrix;
	// modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	
	// glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, texture_smiley);
	// glUniform1i(textureSamplerUniform, 0);
	// glBindVertexArray(vao);
	                                                            
	// // Game coding here
	// glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	                  
	// glBindVertexArray(0);
	// glBindTexture(GL_TEXTURE_2D, 0);
        
	// // unuse the shaderProgramObject
	// glUseProgram(0);

	renderFrame(1.0f);

	glXSwapBuffers(display, window);
}


void uninitialize(void)
{
	// code
	// shader uninitialization
	if (texture_smiley)
	{
		glDeleteTextures(1, &texture_smiley);
		texture_smiley = 0;
	}
	
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}
	                                        
	// deletion and uninitialization of vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	
	
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
	                                                                                                           
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *shaderObjects = NULL;
		shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
	
	GLXContext currentContext;
	currentContext = glXGetCurrentContext();
	if (currentContext && currentContext == glxContext)
	{
		glXMakeCurrent(display, 0, 0);
	}

	if (glxContext)
	{
		glXDestroyContext(display, glxContext);
		glxContext = NULL;
	}

	if (visualInfo)
	{
		free(visualInfo);
	}

	if (window)
	{
		XDestroyWindow(display, window);
	}

	if (colorMap)
	{
		XFreeColormap(display, colorMap);
	}

	if (gpFile)
	{
		free(gpFile);
		gpFile = NULL;
	}

	if (display)	
	{
		XCloseDisplay(display);
		display = NULL;
	}

}


