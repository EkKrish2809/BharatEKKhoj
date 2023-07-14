#pragma once

#include <stdio.h> // For fopen_s(), fprintf(), fclose()
#include "GLHeadersAndMacros.h"

bool CreateLogFile(void);
void printGLInfo(void);
void PrintLog(const char *, ...);
void CloseLogFile(void);

