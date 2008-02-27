#include "glwin.h"

#include <stdio.h>

#ifndef _WIN32
void setupWinGLFunctions()
{
	printf("hi\n");
}
#else

PFNGLBINDBUFFERPROC pglBindBuffer = NULL;
PFNGLGENBUFFERSPROC pglGenBuffers = NULL;
PFNGLBUFFERDATAPROC pglBufferData = NULL;
PFNGLDELETEBUFFERSPROC pglDeleteBuffers = NULL;
PFNGLDRAWRANGEELEMENTSPROC pglDrawRangeElements = NULL;

void setupWinGLFunctions()
{
	printf("1\n");
	static bool alreadySetup = false;
	if (alreadySetup)
		return;
	alreadySetup = true;
	printf("2\n");
	
	pglBindBuffer = (PFNGLBINDBUFFERPROC) wglGetProcAddress("glBindBuffer");
	pglGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	pglBufferData = (PFNGLBUFFERDATAPROC) wglGetProcAddress("glBufferData");
	pglDeleteBuffers = (PFNGLDELETEBUFFERSPROC) wglGetProcAddress("glDeleteBuffers");
	pglDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC) wglGetProcAddress("glDrawRangeElements");
}

#endif
