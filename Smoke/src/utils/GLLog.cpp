#include "GLLog.h"
#include <stdarg.h>
//global variable declarations
FILE *file = NULL;

bool CreateLogFile(void)
{
	//code
	if (file != NULL)
	{
		printf("File poiinter is not NULL ...\n");
		return(false);
	}

	file = fopen("Log.txt", "w");
	if (file == NULL)
	{
		printf("Failed To Create Log File \"Log.txt\" !!! Exitting Now ...\n");
		return(false);
	}

	else
	{
		fprintf(file, "============================================================\n");
		fprintf(file, "Log File \"Log.txt\" Has Been Created Successfully !!!\n");
		fprintf(file, "============================================================\n\n");
	}
	return(true);
}

void printGLInfo(void)
{
	// variable declarations
	GLint numExtensions, i;

	// code
	fprintf(file, "\n");
	fprintf(file, "OpenGL Related Information : \n");
	fprintf(file, "===============================\n\n");
	fprintf(file, "OpenGL Vendor   : %s\n", glGetString(GL_VENDOR));
	fprintf(file, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(file, "OpenGL Version  : %s\n", glGetString(GL_VERSION));
	fprintf(file, "GLSL Version    : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	fprintf(file, "Number Of Supportrd Extensions = %d\n", numExtensions);

	for (i = 0; i < numExtensions; i++)
	{
		fprintf(file, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(file, "===============================\n\n");
}

void PrintLog(const char *fmt, ...)
{
	//variable declarations
	va_list arg;
	int ret;

	//code
	if (file == NULL)
		return;
	va_start(arg, fmt);
	ret = vfprintf(file, fmt, arg);
	va_end(arg);
}

void CloseLogFile(void)
{
	//code
	if (file == NULL)
		return;

	fprintf(file, "============================================================\n");
	fprintf(file, "Log File \"Log.txt\" Has Been Closed Successfully !!!\n");
	fprintf(file, "============================================================\n");

	fclose(file);
	file = NULL;
}

