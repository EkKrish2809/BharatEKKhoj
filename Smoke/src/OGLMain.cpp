// SHREE
// Standard header files
#include <stdio.h>  // For standard IO
#include <stdlib.h> // For exit
#include <memory.h> // For memset

// X11 headers
#include <X11/Xlib.h>   // XClient API
#include <X11/Xutil.h>  // Special Utility, XVisualInfo
#include <X11/XKBlib.h> // Keyboard

// Texture library header
// #include <SOIL/SOIL.h>

#include "./utils/GLHeadersAndMacros.h"
#include "./utils/GLShaders.h"
#include "./utils/GLLog.h"
#include "./utils/common.h"

#include "../includes/vmath.h"
using namespace vmath;

#include "./scenes/BharatEkKhoj.h"

#define STB_IMAGE_IMPLEMENTATION
// #include "../includes/std_image.h"



// CAMERA
#include "./utils/Camera.h"
#include "./utils/BezierCamera.h"

// #include "icon.bmp"

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Function prototype
void toggleFullscreen(void);


vec3 eye = vec3(0.0f, 0.0f, 6.0f);
vec3 center = vec3(0.0f, 0.0f, 0.0f);
vec3 up = vec3(0.0f, 1.0f, 0.0f);

mat4 perspectiveProjectionMatrix;

mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();


// Global Variables
Display *display = NULL;        // 77 Member struct
XVisualInfo *visualInfo = NULL; // 10 member struct
Colormap colormap;
Window window;
Bool fullscreen = False;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *); // PP | Jo function aplyala bhetnar ahe tyacha nav | Architectural Review Board
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;
GLXContext glxContext;

Bool bActiveWindow = False;
GLuint shaderProgramObject;

// Camera
Camera camera;
BezierCamera bezierCamera;

// global resolution
int giWindowWidth, giWindowHeight;

BharatEkKhoj *bharatEkKhoj = NULL;

bool USE_FPV_CAM = false;
int scrollDelta = 0;

// EntryPoint Function
int main(void)
{
	// Function Declarations
	// void update(void);
	// void toggleFullscreen(void);
	int initialize(void);
	void setIconToWindow(void);
	void resize(int, int);
	void draw(void);
	void update(void);
	void uninitialize(void);

	// Local Variables
	int defaultScreen;
	int defaultDepth;

	GLXFBConfig *glxFBConfigs = NULL;
	GLXFBConfig bestGLxFBConfig;
	XVisualInfo *tempXVisualInfo = NULL;
	int numFBConfigs;

	XSetWindowAttributes windowAttributes;
	int styleMask;
	Atom wm_delete_window_atom;
	XEvent event;
	KeySym keysym;
	int screenWidth;
	int screenHeight;
	char keys[26];
	static int winWidth;
	static int winHeight;
	int iRetVal = 0;

	static int frameBufferAttributes[] = // Loke pfd  // OpenGL Change
		{
			GLX_X_RENDERABLE, True,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			GLX_STENCIL_SIZE, 8,
			GLX_DEPTH_SIZE, 24,
			GLX_DOUBLEBUFFER, True,
			None

		};

	Bool bDone = False;

	// Code
	printf("Creating Log File\n");
	
	if(CreateLogFile() == False)
	{
		printf("ERROR : Failed To Create Log File\n");
		exit(0);
	}

	printf("Opening the Display\n");

	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		PrintLog("ERROR : XOpenDisplay() Failed\n");
		uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(display);
	defaultDepth = XDefaultDepth(display, defaultScreen);

	glxFBConfigs = glXChooseFBConfig(display, defaultScreen, frameBufferAttributes, &numFBConfigs);
	if (glxFBConfigs == NULL)
	{
		PrintLog("glXFBConfigs() Failed\n");
		uninitialize();
		exit(1);
	}

	PrintLog("Found Frame Buffers Configs are %d\n", numFBConfigs);

	// Find Best Frame Buffer Config From The Array
	int bestFrameBufferConfig = -1;
	int worstFrameBufferConfig = -1;
	int bestNumberOfSamples = -1;
	int worstNumberOfSamples = 999;

	for (int i = 0; i < numFBConfigs; i++)
	{
		tempXVisualInfo = glXGetVisualFromFBConfig(display, glxFBConfigs[i]);

		if (tempXVisualInfo != NULL)
		{
			int samples, sampleBuffers;

			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLES, &samples);

			PrintLog("Visual Info = 0X%lu Found Sample Buffers =%d & Samples = %d\n", tempXVisualInfo->visualid, sampleBuffers, samples);

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
		XFree(tempXVisualInfo);
		tempXVisualInfo = NULL;
	}

	bestGLxFBConfig = glxFBConfigs[bestFrameBufferConfig];
	glxFBConfig = bestGLxFBConfig;

	XFree(glxFBConfigs);
	glxFBConfigs = NULL;

	visualInfo = glXGetVisualFromFBConfig(display, bestGLxFBConfig);

	PrintLog("Visual Id of Best Visual Info is 0X%lu \n", visualInfo->visualid);

	memset(&windowAttributes, 0, sizeof(XSetWindowAttributes));
	windowAttributes.border_pixel = 0;
	windowAttributes.background_pixel = XBlackPixel(display, defaultScreen);
	windowAttributes.background_pixmap = 0;
	windowAttributes.colormap = XCreateColormap(display,
												RootWindow(display, visualInfo->screen),
												visualInfo->visual,
												AllocNone);
	windowAttributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask | FocusChangeMask | PointerMotionMask | ButtonMotionMask | ButtonPressMask;
	colormap = windowAttributes.colormap;
	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;
	window = XCreateWindow(display,
						   RootWindow(display, visualInfo->screen),
						   0,
						   0,
						   WIN_WIDTH,
						   WIN_HEIGHT,
						   0,
						   visualInfo->depth,
						   InputOutput,
						   visualInfo->visual,
						   styleMask,
						   &windowAttributes);

	if (!window)
	{
		printf("ERROR : XCreateWindow() Failed\n");
		uninitialize();
		exit(1);
	}

	XStoreName(display, window, "Template for BIRTHDAY : OpenGL");
	wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(display, window, &wm_delete_window_atom, 1);
	XMapWindow(display, window);

	// setting icon
	// setIconToWindow();

	// Centering of Window
	screenWidth = XWidthOfScreen(XScreenOfDisplay(display, defaultScreen));
	screenHeight = XHeightOfScreen(XScreenOfDisplay(display, defaultScreen));
	XMoveWindow(display, window, screenWidth / 2 - WIN_WIDTH / 2, screenHeight / 2 - WIN_HEIGHT / 2);

	printf("Starting initialization() \n");
	iRetVal = initialize();
	printf("Completed initialization() \n");
	if (iRetVal == -1)
	{
		PrintLog("GLEW Initialization Failed \n");
		uninitialize();
	}

	if (iRetVal == -2)
	{
		PrintLog("ThisIsJustABeginnig::initialize() Failed \n");
		uninitialize();
	}

	else
	{
		PrintLog("initialze() Succeeded\n");
	}

	// Game Loop

	while (bDone == False)
	{
		
		while (XPending(display)) // XPending means Peak Message
		{
			XNextEvent(display, &event); // XNextEvent means Get Message
			switch (event.type)
			{
			case MapNotify:
				break;

			case FocusIn:
				bActiveWindow = True;
				break;

			case FocusOut:
				bActiveWindow = True;
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
				camera.keyboardInputs(keys[0]);
				switch (keys[0])
				{
				case 'F':
				case 'f':
					if (fullscreen == False)
					{
						toggleFullscreen();
						fullscreen = True;
					}
					else
					{
						toggleFullscreen();
						fullscreen = False;
					}
					break;
				case 'c':
					if (USE_FPV_CAM)
						USE_FPV_CAM = false;
					else
						USE_FPV_CAM = true;
					break;

				}
				break;
			case MotionNotify:
				camera.mouseInputs((float)event.xmotion.x, (float)event.xmotion.y);
				// printf("MouseX: %f\t MouseY: %f\n", (float)event.xmotion.x, (float)event.xmotion.y);
				break;
			case ButtonPress:
				if (event.xbutton.button == 4)
					scrollDelta+=1;
				else if (event.xbutton.button == 5)
					scrollDelta-=1;

				camera.mouseScroll(scrollDelta);
				printf("Scroll Motion: %d \t delta = %d\n",event.xbutton.button, scrollDelta);
				break;

			case ConfigureNotify:
				winWidth = event.xconfigure.width;
				winHeight = event.xconfigure.height;
				resize(winWidth, winHeight); // LOWARD & HIWORD
				break;

			case 33:
				bDone = True;
				break;
			}
		}

		// Here There Should Be Call To Update

		if (bActiveWindow == True)
		{
			draw();

			update();
		}
	}

	uninitialize();
	return (0);
}

void setIconToWindow(void)
{
	// code
	
	// // pointer to WM hints structure
	// XWMHints *window_hints = NULL;

	// // Load the given bitmap data and create an X pixmap containig it
	// Pixmap icon_pixmap = XCreateBitmapFromData(display,
	// 					window,
	// 					icon_bitmap_bits,
	// 					icon_bitmap_width,
	// 					icon_bitmap_height);
	// if (!icon_pixmap)
	// {
	// 	PrintLog("XCreateBitmapFromData() :: error creating pixmap\n");
	// }

	// // Allocate a WM Hints structure
	// window_hints = XAllocWMHints();
	// if (!window_hints)
	// {
	// 	PrintLog("XAllocWMHints() :: out of memory\n");
	// }

	// // Initialize the structure appropriately
	// // 1. specify which size hints we want to fill in. In our case - setting the icon's pixmap
	// window_hints->flags = IconPixmapHint;
	// // 2. specify the desired hints data. In our case - supply the icon's desired pixmap
	// window_hints->icon_pixmap = icon_pixmap;

	// // Pass the hints to the Window Manager
	// XSetWMHints(display, window, window_hints);

	// // Free the WMHints structure
	// XFree(window_hints);
}

void toggleFullscreen(void)
{
    // Full Screen he window manager karto
    // It is network compliant
    // Local variables
    Atom wm_current_state_atom;
    Atom wm_fullscreen_state_atom;
    XEvent event;

    // Code
    wm_current_state_atom = XInternAtom(display, "_NET_WM_STATE", False);
    wm_fullscreen_state_atom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

    memset(&event, 0, sizeof(XEvent));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = wm_current_state_atom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = fullscreen ? 0 : 1;
    event.xclient.data.l[1] = wm_fullscreen_state_atom;

    // SendMessage
    XSendEvent(
        display,
        RootWindow(display, visualInfo->screen),
        False, // Ha message child window la propagate karychi ki nahi
        SubstructureNotifyMask,
        &event);
}

int initialize(void)
{
    void resize(int, int);
    void uninitialize(void);
    void printGLInfo(void);

    // ----------------------------------------------------------------------- PP CODE
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");
    if (glXCreateContextAttribsARB == NULL)
    {
        PrintLog("glXGetProcAddressARB FAILED\n");
        uninitialize();
        exit(1);
    }

    GLint contextAttributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None};

    glxContext = glXCreateContextAttribsARB(display, glxFBConfig, NULL, True, contextAttributes);
    if (!glxContext)
    {
        GLint contextAttributes[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None};
        glxContext = glXCreateContextAttribsARB(display, glxFBConfig, NULL, True, contextAttributes);

        PrintLog("glXCreateContextAttribsARB FAILED, Cannot support 4.6, and hence falling back to default version\n");
    }
    else
    {
        PrintLog("glXCreateContextAttribsARB Passed. Got the context of version 4.6\n");
    }

    if (!glXIsDirect(display, glxContext))
    {
        PrintLog("Direct rendering / HW rendering is not supported\n");
    }
    else
    {
        PrintLog("Direct rendering / HW rendering is supported\n");
    }
    // ------------------------------------------ PP Code end

    glXMakeCurrent(display,window,glxContext);

    // GLEW Initialization
    if (glewInit() != GLEW_OK)
    {
        return -1;
    }

	// printGLInfo();
    
	// Initialize ThisIsJustABeginning
	bharatEkKhoj = new BharatEkKhoj();
	if (bharatEkKhoj->initialize() == false)
	{
		return -2;
	}

    // Depth related changes
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

	// Enabaling the texture
	glEnable(GL_TEXTURE_3D);

    // Here OpenGL functions start
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    perspectiveProjectionMatrix = mat4::identity();

	// Setting up camera
	float _position[] = {0.0, 0.0, 10.0};
	camera = Camera(WIN_WIDTH, WIN_HEIGHT, _position);

	// Setting up bezierCamera
	bezierCamera.initialize();

    // Warmup resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;
}


void resize(int width, int height)
{
    // Code
    // To avoid divided by zero in future.
    if (height == 0)
        height = 1;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// updating global resolution
	giWindowWidth = width;
	giWindowHeight = height;

	// updating camera's resolution
	camera.updateResolution((float)width, (float)height);

    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

// int frameNumber = 0;

void draw(void)
{
    // Code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setting up bezierCamera points
	vector<vector<float>> bezierPoints = {
		{0.0f, 0.0f, 50.0f},
		{-12.0f, 0.0f, 20.0f},
		{20.0f, 0.0f, 10.0f},
		{30.0f, 0.0f, -10.0f},
		{0.0f, 0.0f, -10.0f},
		};

	vector<float> yawPoints = {-90.0f, -110.0f, -130.0f, -180.0f, -270.0f};
	vector<float> pitchPoints = {0.0f, 10.0f, 0.0f, -10.0f};
	bezierCamera.setBezierPoints(bezierPoints, yawPoints, pitchPoints);
	bezierCamera.update();

	if (USE_FPV_CAM)
		viewMatrix = camera.getViewMatrix();
	else
		viewMatrix = bezierCamera.getViewMatrix();

	pushMatrix(modelMatrix);
	{
		// bezierCamera.displayBezierCurve();
	}
	modelMatrix = popMatrix();

    pushMatrix(modelMatrix);
	{
		bharatEkKhoj->display();
	}
	modelMatrix = popMatrix();

	// printf("Main Display !!!\n");
    glXSwapBuffers(display, window);
  
}

void update(void)
{
	// code
	// Update bezierCamera time
	if (bezierCamera.time < 1.0)
	{
		bezierCamera.time += 0.0001f;
	}

	bharatEkKhoj->update();
}

void uninitialize(void)
{
    // Deletion and uninitialization of VBO
	// CloseLogFile();

	bharatEkKhoj->uninitialize();

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
        visualInfo = NULL;
    }
    if (window)
    {
        XDestroyWindow(display, window);
    }
    if (colormap)
    {
        XFreeColormap(display, colormap);
    }
    if (display != NULL)
    {
        XCloseDisplay(display);
        display = NULL;
    }
}


