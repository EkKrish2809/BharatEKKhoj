// SHREE
// Standard header files
#include <stdio.h>	// For standard IO
#include <stdlib.h> // For exit
#include <memory.h> // For memset

// X11 headers
#include <X11/Xlib.h>	// XClient API
#include <X11/Xutil.h>	// Special Utility, XVisualInfo
#include <X11/XKBlib.h> // Keyboard

// Texture library header
// #include <SOIL/SOIL.h>

#include "./utils/GLHeadersAndMacros.h"
#include "./utils/GLShaders.h"
#include "./utils/GLLog.h"
#include "./utils/common.h"

#include "../includes/vmath.h"
using namespace vmath;

#define STB_IMAGE_IMPLEMENTATION
#include "../includes/std_image.h"

#include "./scenes/BharatEkKhoj.h"
#include "../includes/Audio.h"

// CAMERA
#include "./utils/Camera.h"
#include "./utils/BezierCamera.h"

// #include "icon.bmp"

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Function prototype
void toggleFullscreen(void);

BharatEkKhoj *bharatEkKhoj = NULL;

vec3 eye = vec3(0.0f, 0.0f, 6.0f);
vec3 center = vec3(0.0f, 0.0f, 0.0f);
vec3 up = vec3(0.0f, 1.0f, 0.0f);

mat4 perspectiveProjectionMatrix;

mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();

// Camera
Camera camera;
BezierCamera *bezierCamera = NULL;//new BezierCamera();

// Object
float objX = 0.0;
float objY = 0.0;
float objZ = 0.0;
float objIncrement = 1.0f;
// float objIncrement = 0.01f;

// Scale
float scaleX = 0.0;
float scaleY = 0.0;
float scaleZ = 0.0;
float scaleIncrement = 0.1f;


#include "./scenes/SceneOne.h"

vector<vector<float>> bezierPoints = bezierPointsOne;
vector<float> yawPoints = yawPointsOne;
vector<float> pitchPoints = pitchPointsOne;

// vector<vector<float>> bezierPoints = bezierPointsThree;
// vector<float> yawPoints = yawPointsThree;
// vector<float> pitchPoints = pitchPointsThree;
// Setting up bezierCamera points
// vector<vector<float>> bezierPoints = {
// 	// {22.000000f, 0.000000f, 16.000000f},
// 	// {-1.000000f, 0.000000f, 16.000000f},
// 	// {-1.000000f, 0.000000f, 4.000000f},
// 	// {-1.000000f, 0.000000f, 4.000000f},
// 	// {-1.000000f, 0.000000f, 4.000000f},
// 	// {1.399999f, 0.000000f, -1.139996f},
// 	// {10.780056f, 0.020000f, -1.239996f},
// 	// {16.880196f, 0.020000f, 2.860001f},
// 	// {16.880196f, 0.020000f, 2.860001f},
// 	// {16.880196f, 0.020000f, 2.860001f},
// 	// {18.720238f, 0.020000f, 0.840003f},
// 	// {18.720238f, 0.020000f, 0.840003f},     /* SCENE 1 CAMERA*/

//     {0.0f, 11.0f, 0.0f}, 
	
// };

// vector<float> yawPoints = {
// 					// -90.000000f, -90.000000f, -90.000000f, 
// 					// -90.000000f,
// 					// -90.000000f, 
// 					// -90.000000f, 
// 					// -90.000000f, 
// 					// -51.000000f, 
// 					// -51.000000f, 
// 					// -51.000000f, 
// 					// -51.000000f, 
// 					// -51.000000f, 			/* SCENE 1 YAW*/

//                     -90.0f,

// 					};
// vector<float> pitchPoints = {
// 					// 0.000000f, 0.000000f, 0.000000f, 
// 					// 0.000000f, 
// 					// 0.000000f, 
// 					// 0.000000f, 
// 					// 0.000000f, 
// 					// 0.000000f, 
// 					// 0.000000f,
// 					// 0.000000f, 
// 					// 0.000000f, 
// 					// 0.000000f, 				/* SCENE 1 PITCH*/

//                     0.0f,
// 					};



int vectorIndex = bezierPoints.size() - 1;

// Global Variables
Display *display = NULL;		// 77 Member struct
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



// global resolution
int giWindowWidth, giWindowHeight;

bool enableBazierControl = false;

bool USE_FPV_CAM = false;
int scrollDelta = 0;

StopWatchInterface *timer = NULL;
float ELAPSED_TIME = 0.0f;

int current_scene = 0; /* Change this to manually test the scene */

// Audio
Audio audio;

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

	if (CreateLogFile() == False)
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

	XStoreName(display, window, "GLEA : Bharat Ek Khoj !!!");
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
		PrintLog("BharatEkKhoj::initialize() Failed \n");
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
				objX = bezierPoints[bezierPoints.size() - 1][0];
				objY = bezierPoints[bezierPoints.size() - 1][1];
				objZ = bezierPoints[bezierPoints.size() - 1][2];
				scaleX = yawPoints[bezierPoints.size() - 1];
				scaleY = pitchPoints[bezierPoints.size() - 1];

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
					for (int i = 0; i < bezierPoints.size(); i++)
					{
						printf("{%ff, %ff, %ff},\n", bezierPoints[i][0], bezierPoints[i][1], bezierPoints[i][2]);
					}
					printf("\nYaw\n");
					for (int i = 0; i < yawPoints.size(); i++)
					{
						printf("%ff, \n", yawPoints[i]);
					}
					printf("\nPitch\n");
					for (int i = 0; i < pitchPoints.size(); i++)
					{
						printf("%ff, \n", pitchPoints[i]);
					}
					printf("\n");

					printf("ObjX = %f objY = %f objZ = %f \n", objX, objY, objZ);
					printf("scaleX = %f scaleY = %f scaleZ = %f \n", scaleX, scaleY, scaleZ);
					bDone = True;
					break;

				// case XK_7:
				// printf("7 pressed\n");
				// 	objZ += objIncrement;
				// 	break;
				// case XK_1:
				// printf("1 pressed\n");
				// 	objZ -= objIncrement;
				// 	break;
				// case XK_4:
				// printf("4 pressed\n");
				// 	objX -= objIncrement;
				// 	break;
				// case XK_6:
				// printf("6 pressed\n");
				// 	objX += objIncrement;
				// 	break;
				// case XK_5:
				// printf("5 pressed\n");
				// 	objY -= objIncrement;
				// 	break;
				// case XK_8:
				// printf("8 pressed\n");
				// 	objY += objIncrement;
				// 	break;

				case XK_Up:
					scaleY += scaleIncrement;
					break;
				case XK_Down:
					scaleY -= scaleIncrement;
					break;
				case XK_Right:
					scaleX += scaleIncrement;
					break;
				case XK_Left:
					scaleX -= scaleIncrement;
					break;
				case XK_KP_Add:
					scaleZ += scaleIncrement;
					break;
				case XK_KP_Subtract:
					scaleZ -= scaleIncrement;
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

				case 'K':
					objZ += objIncrement;
					break;
				case 'k':
					objZ -= objIncrement;
					break;
				case 'j':
					objX -= objIncrement;
					break;
				case 'J':
					objX += objIncrement;
					break;
				case 'i':
					objY -= objIncrement;
					break;
				case 'I':
					objY += objIncrement;
					break;

				case 'p':
				case 'P':
					bezierPoints.push_back({objX, objY, objZ});
					printf("X = %f, Y = %f, Z = %f\n", objX, objY, objZ);
					yawPoints.push_back(scaleX);
					pitchPoints.push_back(scaleY);
					vectorIndex = bezierPoints.size() - 1;
					break;
				case 'G':
				case 'g':
					if (bezierPoints.size() > vectorIndex + 1)
					{
						vectorIndex++;
						objX = bezierPoints[vectorIndex][0];
						objY = bezierPoints[vectorIndex][1];
						objZ = bezierPoints[vectorIndex][2];

						scaleX = yawPoints[bezierPoints.size() - 1];
						scaleY = pitchPoints[bezierPoints.size() - 1];
					}
					break;
				case 'B':
				case 'b':
					if (vectorIndex > 0)
					{
						vectorIndex--;
						objX = bezierPoints[vectorIndex][0];
						objY = bezierPoints[vectorIndex][1];
						objZ = bezierPoints[vectorIndex][2];

						scaleX = yawPoints[bezierPoints.size() - 1];
						scaleY = pitchPoints[bezierPoints.size() - 1];
					}
					break;
				case 'm':
				case 'M':
					
					break;
				// case 'r':
				// case 'R':
				// 	bharatEkKhoj->cam1->time = 0.0f;
				}
				// printf("VECTORINDEX %d OBJ %f :: %f :: %f | SCALE %f :: %f \n", thisIsJustABeginning->vectorIndex, thisIsJustABeginning->objX, thisIsJustABeginning->objY, thisIsJustABeginning->objZ, thisIsJustABeginning->scaleX, thisIsJustABeginning->scaleY);
				break;
			case MotionNotify:
				camera.mouseInputs((float)event.xmotion.x, (float)event.xmotion.y);
				// printf("MouseX: %f\t MouseY: %f\n", (float)event.xmotion.x, (float)event.xmotion.y);
				break;
			case ButtonPress:
				if (event.xbutton.button == 4)
					scrollDelta += 1;
				else if (event.xbutton.button == 5)
					scrollDelta -= 1;

				camera.mouseScroll(scrollDelta);
				// printf("Scroll Motion: %d \t delta = %d\n", event.xbutton.button, scrollDelta);
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

	glXMakeCurrent(display, window, glxContext);

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
		printf("Initialization Failed : Bharat Ek Khoj\n");
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
	bezierCamera = new BezierCamera();
	bezierCamera->initialize();

	// bezierCamera->time = 1.0f; /* For Scene CAMERA setting. Have to remove after CAMERA is set */
	// current_scene = bharatEkKhoj->getScene();
	// timer
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	// Warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	// audio
	// audio.initializeAudio();

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

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 20000.0f);
}

// int frameNumber = 0;

void draw(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bezierPoints[vectorIndex][0] = objX;
		// bezierPoints[vectorIndex][1] = objY;
		// bezierPoints[vectorIndex][2] = objZ;
		// yawPoints[vectorIndex] = scaleX;
		// pitchPoints[vectorIndex] = scaleY;
		// // fovGlobal[vectorIndex] = scaleZ;


	bezierCamera->setBezierPoints(bezierPoints, yawPoints, pitchPoints);
	bezierCamera->update();

	if (USE_FPV_CAM)
		viewMatrix = camera.getViewMatrix();
	else
		viewMatrix = bezierCamera->getViewMatrix();

	ELAPSED_TIME = sdkGetTimerValue(&timer);
	ELAPSED_TIME = ELAPSED_TIME / 1000.0f;

	// printf("ELAPSED TIME : %f\n", ELAPSED_TIME);
	float time = ELAPSED_TIME;

	

	pushMatrix(modelMatrix);
	{
		// printf("ELAPSED TIME 1 : %f\n", time);
		bharatEkKhoj->display(time);
	}
	modelMatrix = popMatrix();

	pushMatrix(modelMatrix);
	{
		bezierCamera->displayBezierCurve();
	}
	modelMatrix = popMatrix();

	// printf("Main Display !!!\n");
	glXSwapBuffers(display, window);
}

void update(void)
{
	// loacl variables
	// static int current_scene = 0;
	
	// code
	bharatEkKhoj->update();

	if (current_scene == 0)
	{
		bezierPoints = bezierPointsOne;
		yawPoints = yawPointsOne;
		pitchPoints = pitchPointsOne;

		camera_inc = 0.0001f;
	}
	else if (current_scene == 1)
	{
		bezierPoints = bezierPointsTwo;
		yawPoints = yawPointsTwo;
		pitchPoints = pitchPointsTwo;

		camera_inc = 0.0004f;
	}
	else if (current_scene == 2)
	{
		bezierPoints = bezierPointsThree;
		yawPoints = yawPointsThree;
		pitchPoints = pitchPointsThree;

		camera_inc = 0.0004f;
	}

	// Update bezierCamera time
	if (bezierCamera->time < 1.0)
	{
		bezierCamera->time += camera_inc;
	}
	else
	{
		bezierCamera->time = 0.0f;
		bharatEkKhoj->setScene();
		current_scene = bharatEkKhoj->getScene();
	}
	
	// printf("SCENE in Main Update = %d\n", current_scene);
}

void setGlobalBezierCamera(BezierCamera *bezierCam)
{
	bezierCamera = bezierCam;
}

void uninitialize(void)
{
	// Deletion and uninitialization of VBO
	// CloseLogFile();
	// audio.uninitializeAudio();

	bharatEkKhoj->uninitialize();

	if (timer)
	{
		sdkStopTimer(&timer);
		sdkDeleteTimer(&timer);
		timer = NULL;
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
